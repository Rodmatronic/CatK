#include <catk/types.h>
#include <lib/common.h>

void * memcpy(void * dest, const void * src, size_t count)
{
  asm volatile("cld");
  asm volatile("rep movsb" :: "D" (dest), "S" (src), "c" (count) :);
  return dest;
}

void * memcpy16(void * dest, const void * src, size_t count)
{
  count /= sizeof(uint16_t);
  asm volatile("cld");
  asm volatile("rep movsw" :: "D" (dest), "S" (src), "c" (count) :);
  return dest;
}

void * memcpy32(void * dest, const void * src, size_t count)
{
  count /= sizeof(uint32_t);
  asm volatile("cld");
  asm volatile("rep movsd" :: "D" (dest), "S" (src), "c" (count) :);
  return dest;
}
