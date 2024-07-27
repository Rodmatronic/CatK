#include <catk/mem.h>
#include <lib/common.h>

char * strdup(const char * str)
{
  if(!str)
    return NULL;
  char * ret = (char *)malloc(strlen(str) + 1);
  strcpy(ret, str);
  return ret;
}
