#include <sys/types.h>
#include <lib/common.h>

void * memcpy(void * dest, const void * src, size_t count)
{
  const char * sp = (const char *)src;
  char * dp = (char *)dest;
  for(; count != 0; count--) *dp++ = *sp++;
  return dest;
}
