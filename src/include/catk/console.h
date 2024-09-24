#ifndef __CONSOLE_H
#define __CONSOLE_H

#include <catk/compiler.h>
#include <catk/tty.h>
#include <lib/common.h>

struct tty_struct;

struct consw
{
  void (*clear)(void);
  void (*print)(const char *);
  void (*putc)(const char);
  int (*output_intr)(struct tty_struct *, size_t);
};

struct console_font
{
  uint32_t width, height; /* font size */
  uint32_t char_count;
  uint8_t * data;
};

struct vc_data {
  /* console */
  uint16_t vc_num;              /* console number */
  uint32_t vc_rows;             /* console rows */
  uint32_t vc_cols;             /* console columns */
  uint32_t vc_bpp;              /* console bpp */
  uint32_t vc_pitch;            /* console pitch */
  uint32_t vc_size_row;         /* bytes per row */
  uintptr_t vc_screenbuf;       /* address of buffer */
  struct consw vc_sw;
  /* attributes */
  uint8_t vc_attr;              /* current attributes */
  uint8_t vc_def_color;         /* default colors */
  /* fonts */
  struct console_font vc_font;  /* current vc font */
  /* cursor */
  uint16_t vc_pos;              /* cursor position */
  /* others */
  uint8_t vc_has_color : 1;
};

struct console {
  char name[32];
  struct vc_data data;
  struct device * dev;
};

int console_register(struct console * c);
struct console * console_get(int num);
int console_clear(void);
int console_print(const char * str);
int console_putc(const char c);
int console_init(void);
bool is_console_enabled(void);

#endif
