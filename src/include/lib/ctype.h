#ifndef __CTYPE_H
#define __CTYPE_H

#include <lib/common.h>

static inline bool isdigit(char c)
{
  if (c >= '0' && c <= '9')
    return true;
  return false;
}

#endif
