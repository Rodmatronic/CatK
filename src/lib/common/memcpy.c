#include <catk/types.h>
#include <lib/common.h>

void * memcpy(void * dest, const void * src, size_t count)
{
  const char * sp = (const char *)src;
  char * dp = (char *)dest;
  for(; count != 0; count--) *dp++ = *sp++;
  return dest;
}

void * memcpy16(void * dest, const void * src, size_t count)
{
  const uint16_t * sp = (const uint16_t *)src;
  uint16_t * dp = (uint16_t *)dest;
  for(; count != 0; count--) *dp++ = *sp++;
  return dest;
}

void * memcpy32(void * dest, const void * src, size_t count)
{
  const uint32_t * sp = (const uint32_t *)src;
  uint32_t * dp = (uint32_t *)dest;
  for(; count != 0; count--) *dp++ = *sp++;
  return dest;
}