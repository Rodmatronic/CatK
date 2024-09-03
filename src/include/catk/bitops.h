#ifndef __BITOPS_H
#define __BITOPS_H

#include <catk/math.h>

#define BITS_PER_BYTE 8

#define BITS_PER_TYPE(type)   (sizeof(type) * BITS_PER_BYTE)
#define BITS_TO_U32(num)      (DIV_ROUND_UP(num, BITS_PER_TYPE(uint32_t)))
#define BITS_TO_BYTES(num)    (DIV_ROUND_UP(num, BITS_PER_TYPE(uint8_t)))
#define BYTES_TO_BITS(num)    (num * BITS_PER_BYTE)

#endif
