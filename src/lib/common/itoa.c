#include <catk/types.h>
#include <catk/compiler.h>
#include <lib/common.h>

void reverse(char * str, size_t sz)
{
  int start = 0;
  int end = sz - 1;
  while (start < end)
  {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    end--;
    start++;
  }
}

char * itoa(int value, char * str, int radix)
{
  int temp = value;
  char * rc;
  char * ptr;
  char _unused_ * low;
  if (radix < 2 || radix > 36)
  {
    *str = '\0';
    return str;
  }
  rc = ptr = str;
  low = ptr;
  do
  {
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % radix];
    value /= radix;
  }while (value);

  if (temp < 0 && radix == 10)
  {
    *ptr++ = '-';
  }
  *ptr-- = '\0';
	reverse(rc, strlen(rc));
  return rc;
}
