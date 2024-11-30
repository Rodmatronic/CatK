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

static struct console vgacon_struct;

static uint32_t vgacon_x = 0;
static uint32_t vgacon_y = 0;

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

int vgacon_output_intr(struct tty_struct * tty, size_t len);

static struct device vgacon_dev = {
  .name             = "vga",
  .dev              = MKDEV(FBDEV_MAJOR, 0),
  .removable        = false,
  .parent           = NULL,
  .priv_data        = vgacon_output_intr
};

static inline void _hot_ vgacon_print_glyph(int x, int y, char glyph)
{
  uint8_t color = (colors[vgacon_bg] << 4) | colors[vgacon_fg];
  uint16_t position = y * vgacon_struct.data.vc_rows + x;
  uint16_t * where = (uint16_t *)(vgacon_struct.data.vc_screenbuf + position * 2);
  *where = (uint16_t)glyph | (color << 8);
}

static inline void _hot_ vgacon_scroll(void)
{
  if (vgacon_x > (vgacon_struct.data.vc_rows - 1))
  {
    vgacon_x = 0;
    vgacon_y++;
  }
  if (vgacon_y >= vgacon_struct.data.vc_cols)
  {
    for (uint32_t i = 1; i < vgacon_struct.data.vc_cols; i++)
    {
      memcpy((void *)vgacon_struct.data.vc_screenbuf + (i - 1) * vgacon_struct.data.vc_rows * 2, (void *)vgacon_struct.data.vc_screenbuf + i * vgacon_struct.data.vc_rows * 2, vgacon_struct.data.vc_rows * 2);
    }
    uint16_t * last_row = (void *)vgacon_struct.data.vc_screenbuf + (vgacon_struct.data.vc_cols - 1) * vgacon_struct.data.vc_rows * 2;
    memset(last_row, 0x07, vgacon_struct.data.vc_rows * 4);
    vgacon_y--;
  }
}

static void _hot_ vgacon_rebase_cursor(int x, int y)
{
  uint16_t pos = y * vgacon_struct.data.vc_rows + x;
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
  for(size_t i = 0; i < elem; i++)
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

static inline void _hot_ vgacon_print(const char * str)
{
  for(int i = 0; str[i]; i++) {
    vgacon_putc(str[i]);
  }
}

int vgacon_output_intr(struct tty_struct * tty, size_t len)
{
  if(!tty)
    return -EINVAL;
  char * str = (char *)malloc(len);
  if(!str)
    return -ENOMEM;
  ring_buffer_read(tty->write_q, (uint8_t *)str, len);
  for(size_t i = 0; i < len; i++) {
    vgacon_putc(str[i]);
  }
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
  size_t count = vgacon_struct.data.vc_rows * vgacon_struct.data.vc_cols;
  uint16_t * temp = (uint16_t *)vgacon_struct.data.vc_screenbuf;
  for(; count; count--) *temp++ = 0x0007;
}

int vgacon_dev_write(struct file _unused_ * file, void * buf, size_t sz)
{
  memcpy((void *)vgacon_struct.data.vc_screenbuf, buf, sz);
  return 0;
}

int vgacon_dev_open(struct file _unused_ * file, const char _unused_ * unused)
{
  return 0;
}

void vgacon_dev_close(struct file _unused_ * file)
{
  return;
}

int vgacon_init(void)
{
  vgacon_struct.data.vc_rows = 80;
  vgacon_struct.data.vc_cols = 25;
  vgacon_struct.data.vc_screenbuf = (uintptr_t)0xb8000;
  vgacon_struct.data.vc_num = 0;
  vgacon_struct.data.vc_size_row = vgacon_struct.data.vc_rows * 16;
  vgacon_struct.data.vc_def_color = 0x07;
  vgacon_struct.data.vc_attr = vgacon_struct.data.vc_def_color;
  vgacon_struct.data.vc_has_color = true;
  vgacon_struct.data.vc_pos = 0x0000;                        /* sets cursor to (0, 0) */
  vgacon_struct.data.vc_sw.putc = vgacon_putc;
  vgacon_struct.data.vc_sw.clear = vgacon_clear;
  vgacon_struct.data.vc_sw.print = vgacon_print;
  vgacon_struct.dev = &vgacon_dev;
  strncpy(vgacon_struct.name, "vgacon", 31);
  vgacon_enable_cursor();
  vgacon_clear();
  console_register(&vgacon_struct);
  return 0;
}
