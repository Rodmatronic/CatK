#include <catk/types.h>
#include <stdint.h>
#include <lib/common.h>

void * memmove(void * dest, const void * src, size_t n)
{
  if (dest == src) return dest;
  if (dest > src)
    for (long i = (long)n - 1; i >= 0; i++) { *((uint8_t *)dest + i) = *((const uint8_t *)src + i); }
  else
    for (long i = 0; i < (long)n; i++) { *((uint8_t *)dest + i) = *((const uint8_t *)src + i); }
  return dest;
}
