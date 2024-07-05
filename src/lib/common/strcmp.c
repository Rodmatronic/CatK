#include <lib/common.h>

int strcmp(const char * str1, const char * str2)
{
  size_t len = strlen(str1);
  return memcmp(str1, str2, strlen(str1));
}
