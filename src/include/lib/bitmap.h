#ifndef __BITMAP_H
#define __BITMAP_H

#include <catk/bitops.h>

#define CREATE_BITMAP(name, bits) \
  uint32_t name[BITS_TO_U64(bits)]

#endif
