#include <lib/common.h>
#include <catk/types.h>

void * memset(void * dest, char val, size_t count)
{
  void * addr = dest;
  asm volatile("rep stosb" : "+D"(dest), "+c"(count) : "a"(val) : "memory");
  return addr;
}

void * memset16(void * dest, uint16_t val, size_t count)
{
  void * addr = dest;
  asm volatile("rep stosw" : "+D"(dest), "+c"(count) : "a"(val) : "memory");
  return addr;
}

void * memset32(void * dest, uint32_t val, size_t count)
{
  void * addr = dest;
  asm volatile("rep stosl" : "+D"(dest), "+c"(count) : "a"(val) : "memory");
  return addr;
}
