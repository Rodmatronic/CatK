#include <catk/console.h>
#include <catk/debug.h>
#include <catk/errno.h>
#include <catk/spinlock.h>
#include <catk/core.h>
#include <catk/math.h>
#include <catk/debug.h>
#include <lib/common.h>
#include <config.h>
#include <stdint.h>

SPINLOCK_INIT(console_spinlock);

#define MAX_CONSOLES 3

static struct console * consoles[MAX_CONSOLES];
static bool console_enabled = false;
static int current_console = 0;

extern int fbcon_init(void);
extern int vgacon_init(void);

bool is_console_enabled(void) {
  return console_enabled;
}

int console_register(struct console * c) {
  if(!c) {
    return -EINVAL;
  }
  consoles[c->data.vc_num] = c;
  debug("%s registered as console %d\n", c->name, c->data.vc_num);
  return 0;
}

struct console * console_get(int num)
{
  return consoles[num];
}

int console_clear(void) {
  if(!consoles[current_console]->data.vc_sw.clear) {
    /* how dare you.. you forgot to bind a clear function to the console!! */
    return -ENXIO;
  }
  consoles[current_console]->data.vc_sw.clear();
  return 0;
}

int console_print(const char * str) {
  if(!consoles[current_console]->data.vc_sw.print) {
    /* the dummy who registered the console didnt even bind a print function! */
    return -ENXIO;
  }
  consoles[current_console]->data.vc_sw.print(str);
  return 0;
}

int console_putc(const char c) {
  if(!consoles[current_console]->data.vc_sw.putc) {
    /* the dummy who registered the console didnt even bind a putc function! */
    return -ENXIO;
  }
  consoles[current_console]->data.vc_sw.putc(c);
  return 0;
}

int console_init(void)
{
  int rc;
#ifndef CATK_VIDEO_GENERIC
  rc = fbcon_init();
#else 
  rc = vgacon_init();
#endif
  console_enabled = true;
  return rc;
}
