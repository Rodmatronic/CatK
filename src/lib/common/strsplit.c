#include <lib/common.h>

size_t strsplit(char * str, const char delim)
{
  size_t n = 0; /* number of occurences of delim */
  for(size_t i = 0; str[i]; i++)
  {
    if(str[i] == delim)
    {
      str[i] = '\0'; /* null byte means the string ends */
      n++;
    }
  }
  n++;
  return n;
}
