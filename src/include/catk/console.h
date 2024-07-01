#ifndef __CONSOLE_H
#define __CONSOLE_H

#include <stdint.h>
#include <catk/limits.h>
#include <catk/types.h>
#include <lib/common.h>

struct console_font
{
  uint32_t width, height; /* font size */
  uint32_t char_count;
  uint8_t * data;
};

/* virtual console data */
struct vc_data
{
  /* console */
  uint16_t vc_num;              /* console number */
  uint32_t vc_rows;             /* console rows */
  uint32_t vc_cols;             /* console columns */
  uint32_t vc_size_row;         /* bytes per row */
  uintptr_t vc_screenbuf;       /* address of buffer */
  struct consw * vc_sw;
  /* attributes */
  uint8_t vc_attr;              /* current attributes */
  uint32_t vc_def_color;        /* default colors */
  /* fonts */
  struct console_font vc_font;  /* current vc font */
  /* cursor */
  uint16_t vc_pos;              /* cursor position */
  /* others */
  uint8_t vc_has_color : 1;
};

/* callbacks */
struct consw
{
  char *(*con_startup)(void);
  void  (*con_putc)(char);
  void  (*con_clear)(void);
  void  (*con_color_set)(uint8_t, uint8_t);
};

/* console descriptor */
struct console
{
  char name[NAME_MAX + 1];
  void (*write)(const void * buf, size_t len);
  struct device * dev;
  struct vc_data * data;
};

uint32_t console_get_cols(void);
uint32_t console_get_rows(void);
struct console * get_console(void);

bool is_console_enabled(void);

int console_init(uintptr_t addr);
int console_puts(char * buf);
int console_putc(char c);
int console_color_set(uint8_t fb, uint8_t bg);

#endif
