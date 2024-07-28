#include <catk/console.h>
#include <catk/errno.h>
#include <catk/spinlock.h>
#include <lib/common.h>
#include <config.h>
#include <stdint.h>

SPINLOCK_INIT(console_spinlock);

struct console con;

static bool console_enabled = false;

extern int vgacon_init(struct console * con);
extern int fbcon_init(struct console * con, uint32_t addr);

uint32_t console_get_rows(void)
{
  return con.data->vc_rows;
}

uint32_t console_get_cols(void)
{
  return con.data->vc_cols;
}

struct console * get_console(void)
{
  return &con;
}

inline bool is_console_enabled(void)
{
  return console_enabled;
}

inline int console_puts(char * buf)
{
  if(console_enabled)
  {
    if(!con.write)
      return -EIO;
    con.write(buf, strlen(buf));
  }
  return 0;
}

inline int console_putc(char c)
{
  if(console_enabled)
  {
    if(!con.data->vc_sw->con_putc)
      return -EIO;
    con.data->vc_sw->con_putc(c);
  }
  return 0;
}

inline int console_color_set(uint8_t fb, uint8_t bg)
{
  if(!con.data->vc_sw->con_color_set)
    return -EIO;
  con.data->vc_sw->con_color_set(fb, bg);
  return 0;
}

int console_init(uint32_t addr)
{
#if CATK_VIDEO_GENERIC == 1
  vgacon_init(&con);
  console_enabled = true;
#else
  int rc;
  rc = fbcon_init(&con, addr);
  if(IS_ERR(rc))
    return rc;
  console_enabled = true;
#endif
  return 0;
}
