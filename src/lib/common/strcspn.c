#include <lib/common.h>

int strcspn(const char * str, const char * charset)
{
  int len = 0;
  const char * p = str;
  while (*p != '\0')
  {
    const char * q = charset;
    while (*q != '\0')
    {
      if (*p == *q)
        return len;
      q++;
    }
    p++;
    len++;
  }
  return len;
}
