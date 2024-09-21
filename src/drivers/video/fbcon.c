/*

  catk/src/drivers/video/fbcon.c

  The CatK Project 2023 - 2024
  Author: Foo Bar

  Description:
    Default console driver for framebuffer modes (incompatible with some computers).

*/

#include <catk/compiler.h>
#include <catk/console.h>
#include <catk/debug.h>
#include <catk/errno.h>
#include <catk/math.h>
#include <catk/tty.h>
#include <catk/mem.h>
#include <font/term8x16.h>
#include <logo/catk.h>
#include <lib/ring.h>
#include <lib/common.h>
#include <lib/ctype.h>
#include <multiboot2.h>
#include <stdint.h>

#include "ansi.h"

struct console fbcon_struct;
struct multiboot_tag_framebuffer_common * grub_fb = NULL;

static const uint32_t colors[16] = {
  0x172149,
  0xd75151,
  0x00aa00,
  0xaa5500,
  0x0000aa,
  0xaa00aa,
  0x87abab,
  0xbbd3ff,
  /* high intensity colors */
  0x555555,
  0xff6a6a,
  0x55ff55,
  0xffde8e,
  0x5555ff,
  0xff55ff,
  0x99f0f0,
  0xffffff
};

static uint8_t ansi_state = ANSI_STATE_ESC; /* this is set as the default state */
static int ansi_list_idx = 0;
static struct ansi_list ansi_value[8];

static int fbcon_bg = 0;
static int fbcon_fg = 0;
static int fbcon_x = 0;
static int fbcon_y = 0;

static void fbcon_putpx(int x, int y, uint32_t rgb)
{
  uint32_t * buf = (uint32_t *)fbcon_struct.data.vc_screenbuf;
  uint32_t offset = y * (fbcon_struct.data.vc_pitch / 4) + x;
  buf[offset] = rgb;
}

static void fbcon_print_glyph(int con_x, int con_y, uint8_t * glyph)
{
  int x = con_x * fbcon_struct.data.vc_font.width;
  int y = con_y * fbcon_struct.data.vc_font.height;
  for (int dy = 0; dy < fbcon_struct.data.vc_font.height; dy++) 
  {
    for (int dx = 0; dx < fbcon_struct.data.vc_font.width; dx++)
    {
#ifndef FONT_ENDIANNESS_MISMATCH
      int color = (glyph[dy] >> (7 - dx)) & 1;
#else
      int color = (glyph[dy] >> dx) & 1;
#endif
      fbcon_putpx(x + dx, y + dy, color ? colors[fbcon_fg] : colors[fbcon_bg]);
    }
  }
}

static void fbcon_scroll(void)
{
  if (fbcon_x > (fbcon_struct.data.vc_rows / fbcon_struct.data.vc_font.width) - 1)
  {
    fbcon_x = 0;
    fbcon_y++;
  }
  if (fbcon_y > (fbcon_struct.data.vc_cols / fbcon_struct.data.vc_font.height) - 1) // Check if the cursor is at the last row
  {
    // Calculate the size of a single row in bytes
    size_t row_size_bytes = fbcon_struct.data.vc_rows * 4 * fbcon_struct.data.vc_font.height;

    // Calculate the size of all rows except the last one
    size_t all_rows_except_last_size = (fbcon_struct.data.vc_cols / fbcon_struct.data.vc_font.height) * row_size_bytes;

    // Move all rows up by one (excluding the first row)
    memcpy((uint8_t *)fbcon_struct.data.vc_screenbuf, (uint8_t *)fbcon_struct.data.vc_screenbuf + row_size_bytes, all_rows_except_last_size);

    // Clear the last row
    memset32((uint8_t *)fbcon_struct.data.vc_screenbuf + all_rows_except_last_size, colors[0], row_size_bytes);

    // Move the cursor up by one row
    fbcon_y--;
  }
}

static inline void bs(void)
{
  if(fbcon_x)
  {
    fbcon_x--;
    uint8_t * glyph = &fbcon_struct.data.vc_font.data[' ' * fbcon_struct.data.vc_font.height];
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
#ifndef VGAFONT_USED
      uint8_t * glyph = &fbcon_struct.data.vc_font.data[(ch - 32) * fbcon_struct.data.vc_font.height];
#else
      uint8_t * glyph = &fbcon_struct.data.vc_font.data[ch * fbcon_struct.data.vc_font.height];
#endif
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
  uint8_t * glyph = &fbcon_struct.data.vc_font.data[219 * 16];
  fbcon_print_glyph(x, y, glyph);
}

void fbcon_putc(char ch)
{
  int prev_x = fbcon_x, prev_y = fbcon_y; 
  uint8_t * glyph = &fbcon_struct.data.vc_font.data[' ' * 16];
  fbcon_print_glyph(prev_x, prev_y, glyph);
  process_ansi(ch);
  fbcon_rebase_cursor(fbcon_x, fbcon_y, prev_x, prev_y);
}

void fbcon_print(const char * str)
{
  for(int i = 0; str[i]; i++) {
    fbcon_putc(str[i]);
  }
}

int fbcon_output_intr(struct tty_struct * tty, size_t len)
{
  /*
  if(!tty)
    return -EINVAL;
  char * str = (char *)malloc(len);
  if(!str)
    return -ENOMEM;
  ring_buffer_read(tty->write_q, (uint8_t *)str, len);
  for(int i = 0; i < len; i++) {
    fbcon_putc(str[i]);
  }
  free(str);
  */
  return 0;
}

void fbcon_clear(void)
{
  /* thank you rodmatronics for the help! :) */
  memset32((void *)fbcon_struct.data.vc_screenbuf, colors[0], (grub_fb->framebuffer_width * grub_fb->framebuffer_height * 1.2));
}

static inline uint32_t combine_to_uint32_t(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4) {
    return ((uint32_t)byte1 << 24) |
        ((uint32_t)byte2 << 16) |
        ((uint32_t)byte3 << 8)  |
        (uint32_t)byte4;
}

int fbcon_init(void)
{
  int rc;
  grub_fb = (struct multiboot_tag_framebuffer_common *)multiboot2_locate_tag(multiboot2_get_mbi(), MULTIBOOT_TAG_TYPE_FRAMEBUFFER);
  if(!grub_fb)
    return -ENODEV;
  debug("Framebuffer properties:\n");
  debug("Width: %d\n", grub_fb->framebuffer_width);
  debug("Height: %d\n", grub_fb->framebuffer_height);
  debug("Framebuffer address: 0x%016x\n", grub_fb->framebuffer_addr);
  debug("Framebuffer pitch: %d\n", grub_fb->framebuffer_pitch);
  debug("Framebuffer bits per pixel: %d\n", grub_fb->framebuffer_bpp);
  strncpy(fbcon_struct.name, "fbcon", 31);
  fbcon_struct.data.vc_screenbuf = (uintptr_t)grub_fb->framebuffer_addr;
  fbcon_struct.data.vc_bpp = grub_fb->framebuffer_bpp;
  fbcon_struct.data.vc_pitch = grub_fb->framebuffer_pitch;
  fbcon_struct.data.vc_font = font_term8x16;
  fbcon_struct.data.vc_num = 0;
  fbcon_struct.data.vc_rows = grub_fb->framebuffer_width;
  fbcon_struct.data.vc_cols = grub_fb->framebuffer_height;
  fbcon_struct.data.vc_sw.clear = fbcon_clear;
  fbcon_struct.data.vc_sw.print = fbcon_print;
  fbcon_struct.data.vc_sw.putc = fbcon_putc;
  fbcon_struct.data.vc_sw.output_intr = fbcon_output_intr;
  fbcon_x = 0;
  fbcon_y = DIV_ROUND_UP(height, fbcon_struct.data.vc_font.height);
  fbcon_fg = 7;
  fbcon_bg = 0;
  console_register(&fbcon_struct);
  fbcon_clear();

  uint8_t pixel[3];
  for(int y = 0; y < height; y++) {
    for(int x = 0; x < width; x++) {
      HEADER_PIXEL(header_data, pixel);
      fbcon_putpx(x, y, combine_to_uint32_t(0, pixel[0], pixel[1], pixel[2]));
    }
  }
  return 0;
}
