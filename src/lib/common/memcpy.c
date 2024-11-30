#include <catk/types.h>
#include <catk/core.h>
#include <lib/common.h>

void * memcpy(void * dest, const void * src, size_t count)
{
  return platform_memcpy(dest, src, count);
}
