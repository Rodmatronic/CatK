#include <catk/mem.h>
#include <catk/errno.h>
#include <catk/termios.h>
#include <catk/printk.h>
#include <catk/tty.h>
#include <catk/limits.h>
#include <catk/debug.h>
#include <lib/common.h>
#include <catk/types.h>
#include <catk/ipc.h>
#include <catk/task.h>
#include <catk/spinlock.h>
#include <catk/console.h>
#include <lib/ctype.h>

SPINLOCK_INIT(tty_spinlock);

struct tty_struct * ttys[NR_CONSOLES];
struct file_operations tty_fops;

struct tty_struct * tty_lookup(int num) {
  return ttys[num];
}

static void termios_init(struct termios * termios) {
  termios->c_iflag = BRKINT | ICRNL | IXON | IXANY;
  termios->c_oflag = OPOST | ONLCR;
  termios->c_cflag = CREAD | CS8 | HUPCL;
  termios->c_lflag = ISIG | IEXTEN | ICANON | ECHO | ECHOE;
#define CONTROL(char) (((char) - 32) & 0x7f)
  termios->c_cc[VEOF] = CONTROL('d');
  termios->c_cc[VEOL] = CONTROL('?');
  termios->c_cc[VERASE] = '\b';
  termios->c_cc[VINTR] = CONTROL('c');
  termios->c_cc[VKILL] = CONTROL('u');
  termios->c_cc[VMIN] = 1;
  termios->c_cc[VQUIT] = CONTROL('\\');
  termios->c_cc[VSTART] = CONTROL('q');
  termios->c_cc[VSTOP] = CONTROL('s');
  termios->c_cc[VSUSP] = CONTROL('z');
  termios->c_cc[VTIME] = 0;
}

static size_t tty_write(struct tty_struct * tty, const uint8_t * buf, size_t count) {
  spinlock_acquire(&tty_spinlock);
  if(!tty->dev) {
    debug("tty: writing to a non-existent tty struct\n");
    spinlock_release(&tty_spinlock);
    return -ENODEV; 
  }

  char * str = (char *)malloc(count + 1);
  size_t i;

  if(!str) {
    spinlock_release(&tty_spinlock);
    return -ENOMEM;
  }
  strncpy(str, (char *)buf, count);

  for(i = 0; str[i]; i++) {
    ring_buffer_write(tty->write_q, str[i]);
  }

  int (*tty_output_intr)(struct tty_struct *, size_t) = tty->dev->priv_data;
  tty_output_intr(tty, i);
  free(str);
  spinlock_release(&tty_spinlock);
  return i;
}

static size_t tty_read(struct tty_struct * tty, uint8_t * buf, size_t count) {
  spinlock_acquire(&tty_spinlock);
  size_t cnt = count;
  uint8_t ch = 0;
  if(tty->read_q->count < count) {
    while(count > 0) {
      if (ring_buffer_read_single(tty->read_q, &ch) < 0) {
        continue;
      }
      switch (ch) {
        case '\n': {
          ring_buffer_clear(tty->read_q);
          spinlock_release(&tty_spinlock);
          return cnt - count;
        }
        case '\b': {
          if(count < cnt) {
            *--buf = '\0';
            console_putc('\b');
            count++;
          }
          continue;
        }
      }
      *buf++ = ch;
      count--;
    }
  }
  spinlock_release(&tty_spinlock);
  return cnt - count;
}

static int tty_dev_read(struct file * filp, void * buf, size_t sz) {
  assert(MAJOR(filp->rdev) == TTYDEV_MAJOR);
  struct tty_struct * tty = tty_lookup(MINOR(filp->rdev));
  int rc = tty_read(tty, (uint8_t *)buf, sz);
  return rc;
}

static int tty_dev_write(struct file * filp, void * buf, size_t sz) {
  assert(MAJOR(filp->rdev) == TTYDEV_MAJOR);
  struct tty_struct * tty = tty_lookup(MINOR(filp->rdev));
  int rc = tty_write(tty, (const uint8_t *)buf, sz);
  return rc;
}

static int tty_dev_open(struct file _unused_ * filp, const char _unused_ * file) {
  return 0;
}

static void tty_dev_close(struct file _unused_ * filp)
{
  return;
}

static void tty_release(struct tty_struct * tty) {
  if(!tty) {
    return;
  }
  ring_buffer_release(tty->write_q);
  ring_buffer_release(tty->read_q);
  free(tty);
  memset((void *)tty, 0, sizeof(struct tty_struct));
}

static inline void tty_buf_putc(struct ring_buffer * buf, struct tty_struct * tty, int ch) {
  ring_buffer_write(buf, ch);
  if(tty->termios.c_lflag & ECHO) {
    if(ch != '\b') {
      console_putc(ch);
    }
  }
}

/* handles tty input */
void tty_handle_input(struct tty_struct * tty, int ch) {
  if(tty->termios.c_lflag & ISIG && ch == tty->termios.c_cc[VINTR]) {
    if(tty->termios.c_lflag & ECHOCTL) { /* print ctrl character */
      tty_buf_putc(tty->write_q, tty, '^');
      tty_buf_putc(tty->write_q, tty, tty->termios.c_cc[VINTR] + 32);
    }
    dispatch_signal(SIGINT);
    return;
  }
  tty_buf_putc(tty->read_q, tty, ch);
}

/* creates a tty device and binds console device */
int tty_create(int num, struct device * dev) {
  struct device * tty_dev = (struct device *)malloc(sizeof(struct device));
  if(!tty_dev)
    return -ENOMEM;
  if(ttys[num] != NULL) {
    free(tty_dev);
    return -EBUSY;
  }
  struct tty_struct * tty = (struct tty_struct *)malloc(sizeof(struct tty_struct));
  if(tty == NULL) {
    free(tty_dev);
    return -ENOMEM;
  }
  tty->write_q = (struct ring_buffer *)malloc(sizeof(struct ring_buffer));
  tty->read_q = (struct ring_buffer *)malloc(sizeof(struct ring_buffer));
  if(tty->write_q == NULL || tty->read_q == NULL) {
    tty_release(tty);
    free(tty_dev);
    return -ENOMEM;
  }
  termios_init(&tty->termios);
  if (ring_buffer_init(tty->write_q, TTY_BUF_SIZE) < 0 || ring_buffer_init(tty->read_q, TTY_BUF_SIZE) < 0) {
    tty_release(tty);
    free(tty_dev);
    return -ENOMEM;
  }
  tty->winsize.ws_row = console_get(0)->data.vc_rows;
  tty->winsize.ws_col = console_get(0)->data.vc_cols;
  tty->winsize.ws_xpixel = 0;
  tty->winsize.ws_ypixel = 0;
  tty->ops = (struct tty_operations *)malloc(sizeof(struct tty_operations));
  assert(tty->ops != NULL);
  tty->ops->write = tty_write;
  tty->ops->read = tty_read;
  tty->dev = dev; /* console device */
  ttys[num] = tty;
  /* now create the character device */
  snprintf((char *)tty_dev->name, NAME_MAX - 1, "tty%d", num);
  tty_dev->removable = true;
  tty_dev->dev       = MKDEV(TTYDEV_MAJOR, num);
  tty_dev->priv_data = tty;
  chrdev_register(tty_dev, &tty_fops);
  return 0;
}

struct file_operations tty_fops = {
  NULL,
  NULL,
  tty_dev_read,
  tty_dev_write,
  NULL,
  NULL,
  tty_dev_open,
  tty_dev_close
};
