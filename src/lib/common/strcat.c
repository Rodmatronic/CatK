#include <lib/common.h>

char * strcat(char * dest, const char * src)
{
  char * rdest = dest;
  while (*dest)
    dest++;
  memcpy(dest, src, strlen(src) + 1);
  return rdest;
}
