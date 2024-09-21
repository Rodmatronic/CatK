#include <lib/common.h>
#include <catk/types.h>

size_t strlen(const char * str)
{
  int i = 0;
  while(str[i]) i++;
  return i;
}
