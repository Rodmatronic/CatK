#include <catk/console.h>
#include <catk/errno.h>
#include <catk/spinlock.h>
#include <catk/platform.h>
#include <catk/math.h>
#include <catk/debug.h>
#include <lib/common.h>
#include <config.h>
#include <stdint.h>

static struct console * con = NULL;
static bool console_enabled = false;

extern int fbcon_init(void);

int console_register(struct console * c) {
  if(!c) {
    return -EINVAL;
  }
  con = c;
  debug("%s registered as console %d\n", c->name, c->data.vc_num);
  return 0;
}

struct console * console_get(void)
{
  return con;
}

int console_clear(void) {
  if(!con->data.vc_sw.clear) {
    /* how dare you.. you forgot to bind a clear function to the console!! */
    return -ENXIO;
  }
  if(console_enabled)
    con->data.vc_sw.clear();
}

int console_print(const char * str) {
  if(!con->data.vc_sw.print) {
    /* the dummy who registered the console didnt even bind a print function! */
    return -ENXIO;
  }
  if(console_enabled)
    con->data.vc_sw.print(str);
  return 0;
}

int console_putc(const char c) {
  if(!con->data.vc_sw.putc) {
    /* the dummy who registered the console didnt even bind a putc function! */
    return -ENXIO;
  }
  if(console_enabled)
    con->data.vc_sw.putc(c);
  return 0;
}

int console_init(void)
{
  int rc;
  rc = fbcon_init();
  if(IS_ERR(rc)) {
    return rc;
  }
  console_enabled = true;
  return 0;
}
