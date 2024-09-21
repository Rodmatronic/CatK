#include <lib/common.h>

char * strstr(const char * s1, const char * s2)
{
  int n = strlen(s2);
  while(*s1)
  {
    if(!memcmp(s1++, s2, n))
      return (char *)(s1-1);
  }
  return NULL;
}
