#include <lib/common.h>
#include <catk/types.h>
#include <lib/ctype.h>

int atoi(const char * str)
{
  int ret = 0;
  for(int i = 0; i < strlen(str); i++)
  {
    if(!isdigit(str[i]))
      return -1;
    ret *= 10;
    ret += str[i] - '0';
  }
  return ret;
}
