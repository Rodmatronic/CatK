#include <lib/common.h>
#include <stdint.h>

int memcmp(const void * mem1, const void * mem2, size_t n) 
{
  char * m1 = (char *)mem1;
  char * m2 = (char *)mem2;
  size_t ret = 0;
  for(int i = 0; i < n; i++)
    ret += (m1[i] != m2[i]);
  return ret;
}
