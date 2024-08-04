/*

  catk/src/drivers/video/fbcon.c

  The CatK Project 2023 - 2024
  Author: Foo Bar

  Description:
    Default console driver for framebuffer modes (incompatible with some computers).

*/

#include <catk/console.h>
#include <catk/device.h>
#include <catk/compiler.h>
#include <catk/tty.h>
#include <catk/mem.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/spinlock.h>
#include <font/vga8x16.h>
#include <multiboot2.h>
#include <lib/common.h>
#include <lib/ctype.h>
#include <stdint.h>

#include "ansi.h"

static struct vc_data c;
static struct consw cb;

static int fbcon_x = 0;
static int fbcon_y = 0;

static const uint32_t colors[16] = {
  0x000000,
  0xaa0000,
  0x00aa00,
  0xaa5500,
  0x0000aa,
  0xaa00aa,
  0x00aaaa,
  0xffffff, // 0xaaaaaa is the true color
  /* high intensity colors */
  0x555555,
  0xff5555,
  0x55ff55,
  0xffff55,
  0x5555ff,
  0xff55ff,
  0x55ffff,
  0xffffff
};

static uint32_t fbcon_fg = 7;
static uint32_t fbcon_bg = 0;

static uint8_t ansi_state = ANSI_STATE_ESC; /* this is set as the default state */
static int ansi_list_idx = 0;

static struct ansi_list ansi_value[8];
static struct file_operations fbcon_fops;

static inline void fbcon_putc(char c);
void fbcon_clear(void);
void fbcon_color_set(uint8_t fg, uint8_t bg);
int fbcon_output_intr(struct tty_struct * tty, size_t len);

static struct device fbcon_dev = {
  .major            = FBDEV_MAJOR,
  .minors           = 0,  // in a devfs environment, this would be /dev/fb0
  .removable        = false,
  .parent           = NULL,
  .priv_data        = fbcon_output_intr
};

/* startup, and return name */
static char * fbcon_startup(void)
{
  /* use defaults */
  c.vc_num = 0;
  c.vc_size_row = c.vc_rows * 4 * 16;
  c.vc_def_color = 0x07;
  c.vc_font = font_vga_8x16;
  c.vc_attr = c.vc_def_color;
  c.vc_has_color = true;
  c.vc_pos = 0x0000;                        /* sets cursor to (0, 0) */
  cb.con_putc = fbcon_putc;
  cb.con_clear = fbcon_clear;
  cb.con_color_set = fbcon_color_set;
  return "console";
}

static inline void _hot_ fbcon_putpx(int x, int y, uint32_t rgb)
{
  uint32_t * buf = (uint32_t *)c.vc_screenbuf;
  uint32_t offset = y * c.vc_rows + x;
  buf[offset] = rgb;
}

static void _hot_ fbcon_print_glyph(int con_x, int con_y, uint8_t * glyph)
{
  int x = con_x * c.vc_font.width;
  int y = con_y * c.vc_font.height;
  for (int dy = 0; dy < c.vc_font.height; dy++) 
  {
    for (int dx = 0; dx < c.vc_font.width; dx++)
    {
      int color = (glyph[dy] >> (7 - dx)) & 1;
      fbcon_putpx(x + dx, y + dy, color ? colors[fbcon_fg] : colors[fbcon_bg]);
    }
  }
}

static inline void _hot_ fbcon_scroll(void)
{
  if (fbcon_x > (c.vc_rows / c.vc_font.width) - 1)
  {
    fbcon_x = 0;
    fbcon_y++;
  }
  if (fbcon_y > (c.vc_cols / 16) - 1)
  {
    memcpy32((void *)c.vc_screenbuf, (void *)(c.vc_screenbuf + c.vc_size_row), (c.vc_cols / c.vc_font.height) * c.vc_size_row);
    memset32((void *)c.vc_screenbuf + (c.vc_cols / c.vc_font.height) * c.vc_size_row, 0x00000000, c.vc_size_row);
    fbcon_y--;
  }
}

static inline void bs(void)
{
  if(fbcon_x)
  {
    fbcon_x--;
    uint8_t * glyph = &c.vc_font.data[' ' * 16];
    fbcon_print_glyph(fbcon_x, fbcon_y, glyph);
  }
}

static void process_ascii(char ch)
{
  switch(ch)
  {
    case '\n':
    {
      fbcon_y++;
      fbcon_x = 0;
      break;
    }
    case '\t':
    {
      fbcon_x += 2;
      break;
    }
    case 0x08: /* BS (Backspace) */
    {
      bs();
      break;
    }
    default:
    {
      uint8_t * glyph = &c.vc_font.data[ch * 16];
      fbcon_print_glyph(fbcon_x, fbcon_y, glyph);
      fbcon_x++;
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
      fbcon_fg = 7;
      fbcon_bg = 0;
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
        fbcon_fg = num;
      }
      else if(list[i].value >= 40 && list[i].value <= 47)
      {
        uint8_t num;
        if(bg_hi)
          num = (list[i].value - 40) + 8;
        else
          num = list[i].value - 40;
        fbcon_bg = num;
      }
    }
  }
}

static void _hot_ process_ansi(char ch)
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
  fbcon_scroll();
}

static void fbcon_rebase_cursor(int x, int y, int old_x, int old_y)
{
  uint8_t * glyph = &c.vc_font.data[219 * 16];
  fbcon_print_glyph(x, y, glyph);
}

static inline void _hot_ fbcon_putc(char ch)
{
  int prev_x = fbcon_x, prev_y = fbcon_y; 
  uint8_t * glyph = &c.vc_font.data[' ' * 16];
  fbcon_print_glyph(prev_x, prev_y, glyph);
  process_ansi(ch);
  fbcon_rebase_cursor(fbcon_x, fbcon_y, prev_x, prev_y);
}

static inline void _hot_ fbcon_write(const void * buf, size_t len)
{
  char * _buf = (char *)buf;
  for(int i = 0; i < len; i++)
  {
    if(_buf[i])
      fbcon_putc(_buf[i]);
    else
      break;
  }
}

void fbcon_color_set(uint8_t fg, uint8_t bg)
{

}

int fbcon_output_intr(struct tty_struct * tty, size_t len)
{
  if(!tty)
    return -EINVAL;
  char * str = (char *)malloc(len);
  if(!str)
    return -ENOMEM;
  ring_buffer_read(tty->write_q, (uint8_t *)str, len);
  fbcon_write(str, len);
  free(str);
  return 0;
}

void fbcon_clear(void)
{
  memset((void *)c.vc_screenbuf, 0, (c.vc_rows * c.vc_cols));
}

int fbcon_dev_write(struct file * file, void * buf, size_t sz)
{
  return -ENOSYS; // not implemented
}

int fbcon_dev_open(struct file * file, const char * unused)
{
  return 0;
}

void fbcon_dev_close(struct file * file)
{
  return;
}

int fbcon_init(struct console * con, uint32_t addr)
{
  int rc;
  struct multiboot_tag_framebuffer_common * grub_fb = (struct multiboot_tag_framebuffer_common *)multiboot2_locate_tag(addr, MULTIBOOT_TAG_TYPE_FRAMEBUFFER);
  if(!grub_fb)
    return -ENODEV;
  c.vc_rows = grub_fb->framebuffer_width;
  c.vc_cols = grub_fb->framebuffer_height;
  c.vc_screenbuf = (uintptr_t)grub_fb->framebuffer_addr;
  cb.con_startup = fbcon_startup;
  strncpy(con->name, cb.con_startup(), sizeof(con->name));
  strncpy((char *)fbcon_dev.name, con->name, sizeof(con->name));
  con->write = fbcon_write;
  con->data = &c;
  con->dev = &fbcon_dev;
  fbcon_clear();
  rc = register_chrdev(FBDEV_MAJOR, "fb", &fbcon_dev, &fbcon_fops);
  if(IS_ERR(rc))
  {
    printk("Failed to register framebuffer: %d\n", rc);
    return rc;
  }
  return 0;
}

struct file_operations fops = {
  NULL,
  NULL,               /* read */
  fbcon_dev_write,    /* write */
  NULL,               /* readdir */
  NULL,               /* ioctl */
  fbcon_dev_open,     /* open */
  fbcon_dev_close,    /* close */
};
