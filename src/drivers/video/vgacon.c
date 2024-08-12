/*

  catk/src/drivers/video/fbcon.c

  The CatK Project 2023 - 2024
  Author: Foo Bar

  Description:
    Default console driver for framebuffer modes (incompatible with some computers).

*/

#include <catk/console.h>
#include <catk/compiler.h>
#include <catk/device.h>
#include <catk/tty.h>
#include <catk/mem.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <catk/io.h>
#include <lib/common.h>
#include <lib/ctype.h>
#include <stdint.h>

#include "ansi.h"

static struct vc_data c;
static struct consw cb;

static int vgacon_x = 0;
static int vgacon_y = 0;

static const uint8_t colors[16] = {
  0x00,
  0x04,
  0x02,
  0x06,
  0x01,
  0x05,
  0x03,
  0x07,
  /* high intensity colors */
  0x08,
  0x0c,
  0x0a,
  0x0e,
  0x09,
  0x0d,
  0x0b,
  0x0f
};

static uint8_t vgacon_fg = 7;
static uint8_t vgacon_bg = 0;

static uint8_t ansi_state = ANSI_STATE_ESC; /* this is set as the default state */
static int ansi_list_idx = 0;

static struct ansi_list ansi_value[8];
static struct file_operations _unused_ vgacon_fops;

static inline void vgacon_putc(char c);
static inline void vgacon_clear(void);
void vgacon_color_set(uint8_t fg, uint8_t bg);
int vgacon_output_intr(struct tty_struct * tty, size_t len);

/* startup, and return name */
static char * vgacon_startup(void)
{
  /* use defaults */
  c.vc_num = 0;
  c.vc_size_row = c.vc_rows * 4 * 16;
  c.vc_def_color = 0x07;
  c.vc_attr = c.vc_def_color;
  c.vc_has_color = true;
  c.vc_pos = 0x0000;                        /* sets cursor to (0, 0) */
  cb.con_putc = vgacon_putc;
  cb.con_clear = vgacon_clear;
  cb.con_color_set = vgacon_color_set;
  return "console";
}

static inline void _hot_ vgacon_print_glyph(int x, int y, char glyph)
{
  uint8_t color = (colors[vgacon_bg] << 4) | colors[vgacon_fg];
  uint16_t position = y * c.vc_rows + x;
  uint16_t * where = (uint16_t *)(c.vc_screenbuf + position * 2);
  *where = (uint16_t)glyph | (color << 8);
}

static inline void _hot_ vgacon_scroll(void)
{
  if (vgacon_x > (c.vc_rows - 1))
  {
    vgacon_x = 0;
    vgacon_y++;
  }
  if (vgacon_y >= c.vc_cols)
  {
    for (int i = 1; i < c.vc_cols; i++)
    {
      memcpy((void *)c.vc_screenbuf + (i - 1) * c.vc_rows * 2, (void *)c.vc_screenbuf + i * c.vc_rows * 2, c.vc_rows * 2);
    }
    uint16_t * last_row = (void *)c.vc_screenbuf + (c.vc_cols - 1) * c.vc_rows * 2;
    memset(last_row, 0, c.vc_rows * 2);

    vgacon_y--;
  }
}

static void _hot_ vgacon_rebase_cursor(int x, int y)
{
  uint16_t pos = y * c.vc_rows + x;
  outb(0x3d4, 0x0f);
  outb(0x3d5, (uint8_t)(pos & 0xff));
  outb(0x3d4, 0x0e);
  outb(0x3d5, (uint8_t)((pos >> 8) & 0xff));
}

static inline void bs(void)
{
  if(vgacon_x)
  {
    vgacon_x--;
    vgacon_print_glyph(vgacon_x, vgacon_y, ' ');
  }
}

static void process_ascii(char ch)
{
  switch(ch)
  {
    case '\n':
    {
      vgacon_y++;
      vgacon_x = 0;
      break;
    }
    case '\t':
    {
      vgacon_x += 2;
      break;
    }
    case 0x08: /* BS (Backspace) */
    {
      bs();
      break;
    }
    default:
    {
      vgacon_print_glyph(vgacon_x, vgacon_y, ch);
      vgacon_x++;
      break;
    }
  }
}

static void process_ansi_sgr(size_t elem)
{
  static bool fg_hi = false;
  static bool bg_hi = false;
  struct ansi_list * list = &ansi_value[0];
  for(int i = 0; i < elem; i++)
  {
    if (list[i].empty || list[i].value == 0)
    {
      vgacon_fg = 7;
      vgacon_bg = 0;
    }
    else
    {
      if(list[i].value == 1)
      {
        fg_hi = fg_hi ? false : true;
      }
      else if(list[i].value == 21)
      {
        bg_hi = bg_hi ? false : true;
      }
      if(list[i].value >= 30 && list[i].value <= 37)
      {
        uint8_t num;
        if(fg_hi)
          num = (list[i].value - 30) + 8;
        else
          num = list[i].value - 30;
        vgacon_fg = num;
      }
      else if(list[i].value >= 40 && list[i].value <= 47)
      {
        uint8_t num;
        if(bg_hi)
          num = (list[i].value - 40) + 8;
        else
          num = list[i].value - 40;
        vgacon_bg = num;
      }
    }
  }
}

static void process_ansi(char ch)
{
  switch(ansi_state)
  {
    case ANSI_STATE_ESC:
    {
      if(ch == '\033')
      {
        ansi_state = ANSI_STATE_BRACKET;
        ansi_list_idx = 0;
        ansi_value[ansi_list_idx].value = 0;
        ansi_value[ansi_list_idx].empty = true;
      }
      else
      {
        ansi_state = ANSI_STATE_ESC;
        process_ascii(ch);
      }
      break;
    }
    case ANSI_STATE_BRACKET:
    {
      if(ch == '[')
      {
        ansi_state = ANSI_STATE_STARTVAL;
      }
      else
      {
        ansi_state = ANSI_STATE_ESC;
        process_ascii(ch);
      }
      break;
    }
    case ANSI_STATE_STARTVAL:
    {
      if(isdigit(ch))
      {
        ansi_value[ansi_list_idx].value *= 10;
        ansi_value[ansi_list_idx].value += (ch - '0');
        ansi_value[ansi_list_idx].empty = false;
      }
      else
      {
        if(ansi_list_idx < 8)
          ansi_list_idx++;
        ansi_value[ansi_list_idx].value = 0;
        ansi_value[ansi_list_idx].empty = true;
        ansi_state = ANSI_STATE_ENDVAL;
      }
      break;
    }
    default:
    {
      break;
    }
  }
  if(ansi_state == ANSI_STATE_ENDVAL)
  {
    if(ch == ';')
    {
      ansi_state = ANSI_STATE_STARTVAL;
    }
    else
    {
      if(ch == 'm')
      {
        process_ansi_sgr(ansi_list_idx);
      }
      ansi_state = ANSI_STATE_ESC;
    }
  }
  vgacon_scroll();
}

static inline void _hot_ vgacon_putc(char ch)
{
  process_ansi(ch);
  vgacon_rebase_cursor(vgacon_x, vgacon_y);
}

static inline void _hot_ vgacon_write(const void * buf, size_t len)
{
  char * _buf = (char *)buf;
  for(int i = 0; i < len; i++)
  {
    if(_buf[i])
      vgacon_putc(_buf[i]);
    else
      break;
  }
}

void vgacon_color_set(uint8_t fg, uint8_t bg)
{

}

int vgacon_output_intr(struct tty_struct * tty, size_t len)
{
  if(!tty)
    return -EINVAL;
  char * str = (char *)malloc(len);
  if(!str)
    return -ENOMEM;
  ring_buffer_read(tty->write_q, (uint8_t *)str, len);
  vgacon_write(str, len);
  free(str);
  return 0;
}

static void vgacon_enable_cursor(void) {
  outb(0x3D4, 0x0A);
  outb(0x3D5, (inb(0x3D5) & 0xC0) | 0);
  outb(0x3D4, 0x0B);
  outb(0x3D5, (inb(0x3D5) & 0xE0) | 0x0E);
}

static inline void vgacon_clear(void)
{
  memset16((void *)c.vc_screenbuf, 0x0007, c.vc_rows * c.vc_cols);
}

int vgacon_dev_write(struct file * file, void * buf, size_t sz)
{
  memcpy((void *)c.vc_screenbuf, buf, sz);
  return 0;
}

int vgacon_dev_open(struct file * file, const char * unused)
{
  return 0;
}

void vgacon_dev_close(struct file * file)
{
  return;
}

int vgacon_init(struct console * con, uint32_t addr)
{
  c.vc_rows = 80;
  c.vc_cols = 25;
  c.vc_screenbuf = (uintptr_t)0xb8000;
  cb.con_startup = vgacon_startup;
  strncpy(con->name, cb.con_startup(), sizeof(con->name));
  con->write = vgacon_write;
  con->data = &c;
  con->dev = NULL;
  vgacon_enable_cursor();
  vgacon_clear();
  return 0;
}

static struct file_operations _unused_ fops = {
  NULL,
  NULL,               /* read */
  vgacon_dev_write,    /* write */
  NULL,               /* readdir */
  NULL,               /* ioctl */
  vgacon_dev_open,     /* open */
  vgacon_dev_close,    /* close */
};
