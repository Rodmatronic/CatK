#include <catk/mem.h>
#include <catk/errno.h>
#include <catk/termios.h>
#include <catk/printk.h>
#include <catk/tty.h>
#include <catk/debug.h>
#include <lib/common.h>
#include <catk/types.h>

struct tty_struct * ttys[NR_CONSOLES] = {NULL};

struct tty_struct * tty_lookup(int num)
{
  if(!ttys[num])
    return PTR_ERR(-ENODEV);
  return ttys[num];
}

static void termios_init(struct termios * termios)
{
  termios->c_iflag = BRKINT | ICRNL | IXON | IXANY;
  termios->c_oflag = OPOST | ONLCR;
  termios->c_cflag = CREAD | CS8 | HUPCL;
  termios->c_lflag = ISIG | IEXTEN | ICANON | ECHO | ECHOE;
#define CONTROL(char) (((char) - 64) & 0x7f)
  termios->c_cc[VEOF] = CONTROL('D');
  termios->c_cc[VEOL] = CONTROL('?');
  termios->c_cc[VERASE] = '\b';
  termios->c_cc[VINTR] = CONTROL('C');
  termios->c_cc[VKILL] = CONTROL('U');
  termios->c_cc[VMIN] = 1;
  termios->c_cc[VQUIT] = CONTROL('\\');
  termios->c_cc[VSTART] = CONTROL('Q');
  termios->c_cc[VSTOP] = CONTROL('S');
  termios->c_cc[VSUSP] = CONTROL('Z');
  termios->c_cc[VTIME] = 0;
}

size_t tty_read(struct tty_struct * tty, const uint8_t * buf, size_t count)
{
  if(!tty->ops->read)
    return -ENXIO;

  if(!tty->dev)
  {
    debug("[tty_io] reading from a non-existent tty struct\n");
    return -ENODEV; 
  }

  if(!IS_VALID_TTY(tty))
    return -ENOTTY; /* its not a tty device */

  char * str = (char *)malloc(count + 1);
  size_t i;

  if(!str)
    return -ENOMEM;

  ring_buffer_read(tty->read_q, (uint8_t *)str, count);

  int (*tty_output_intr)(struct tty_struct *, size_t) = tty->dev->priv_data;
  tty_output_intr(tty, i);
  free(str);
  return i;
}

size_t tty_write(struct tty_struct * tty, const uint8_t * buf, size_t count)
{
  if(!tty->ops->write)
    return -ENXIO;

  if(!tty->dev)
  {
    debug("[tty_io] writing to a non-existent tty struct\n");
    return -ENODEV; 
  }

  if(!IS_VALID_TTY(tty))
    return -ENOTTY; /* its not a tty device */

  char * str = (char *)malloc(count + 1);
  size_t i;

  if(!str)
    return -ENOMEM;
  strncpy(str, (char *)buf, count);

  for(i = 0; str[i]; i++)
    ring_buffer_write(tty->write_q, str[i]);

  int (*tty_output_intr)(struct tty_struct *, size_t) = tty->dev->priv_data;
  tty_output_intr(tty, i);
  free(str);
  return i;
}

static void tty_release(struct tty_struct * tty)
{
  if(!tty)
  {
    debug("[tty_io] releasing a tty struct that is null??\n");
    return;
  }
  ring_buffer_release(tty->write_q);
  ring_buffer_release(tty->read_q);
  free(tty);
  memset((void *)tty, 0, sizeof(struct tty_struct));
}

int tty_create(int num, struct device * dev)
{
  if(ttys[num])
  {
    debug("[tty_io] tty%d device already exists, and is in use\n", num);
    return -EBUSY;
  }
  struct tty_struct * tty = (struct tty_struct *)malloc(sizeof(struct tty_struct));
  if(!tty)
  {
    tty_release(tty);
    return -ENOMEM;
  }
  memset((void *)tty, 0, sizeof(struct tty_struct));
  memset((void *)&tty->termios, 0, sizeof(struct termios));
  termios_init(&tty->termios);
  if (ring_buffer_init(tty->write_q, TTY_BUF_SIZE) < 0)
  {
    goto ring_mem_err;
  }
  if(ring_buffer_init(tty->read_q, TTY_BUF_SIZE) < 0)
  {
    goto ring_mem_err;
  }
  tty->winsize.ws_row = console_get_rows();
  tty->winsize.ws_col = console_get_cols();
  tty->winsize.ws_xpixel = 0;
  tty->winsize.ws_ypixel = 0;
  tty->ops->write = tty_write;
  tty->dev = dev;
  ttys[num] = tty;
  debug("[tty_io] tty%d created\n", num);
  return 0;

ring_mem_err:
  tty_release(tty);
  return -ENOMEM;
}
