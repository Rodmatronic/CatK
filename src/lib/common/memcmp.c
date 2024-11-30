#include <lib/common.h>

int memcmp(const void * addr1, const void * addr2, size_t count) {
  char * m1 = (char *)addr1;
  char * m2 = (char *)addr2;
  size_t ret = 0;
  for(size_t i = 0; i < count; i++)
    ret += (m1[i] != m2[i]);
  return ret;
}
