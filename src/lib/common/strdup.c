#include <catk/mem.h>
#include <lib/common.h>

char * strdup(char * str)
{
  if(!str)
    return NULL;
  char * ret = (char *)malloc(strlen(str) + 1);
  memcpy(ret, str, strlen(str));
  ret[strlen(str) + 1] = '\0';
  return ret;
}
