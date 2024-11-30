#include <lib/common.h>
#include <catk/types.h>
#include <catk/core.h>

void * memset(void * dest, char val, size_t count)
{
  return platform_memset(dest, val, count);
}
