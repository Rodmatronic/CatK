#ifndef __ANSI_H
#define __ANSI_H
 
#include <lib/common.h>

enum ansi_state
{
  ANSI_STATE_ESC,
  ANSI_STATE_BRACKET,
  ANSI_STATE_STARTVAL,
  ANSI_STATE_ENDVAL
};

struct ansi_list
{
  uint8_t value;
  bool empty;
};

#endif
