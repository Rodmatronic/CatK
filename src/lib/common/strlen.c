#include <lib/common.h>
#include <sys/types.h>

size_t strlen(const char * str)
{
  int i = 0;
  while(str[i]) i++;
  return i;
}
