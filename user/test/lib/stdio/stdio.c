#include <stdarg.h>
#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"

#define BIT(n) (1 << n)

#define FLAG_FILL_ZERO BIT(0)

#define SET_FLAG(f) flags |= f
#define UNSET_FLAG(f) flags &= ~f

#define HAS_FLAG(f) (flags & f)

#define PUTS(s) \
  for(int i = 0; s[i]; i++) \
  { \
    str[str_i++] = s[i]; \
    n++; \
  }

int vsnprintf(char * str, size_t len, const char format[], va_list arg)
{
  int flags = 0;
  int n = 0;
  int str_i = 0;
  size_t width = 0;
  for(int i = 0; format[i]; i++)
  {
    if(format[i] == '%')
    {
      i++;
next:
      if(!format[i])
        break;
      switch(format[i])
      {
        case '0'...'9':
        {
          if (format[i] == '0' && !width)
            SET_FLAG(FLAG_FILL_ZERO);
          width *= 10;
          width += format[i] - '0';
          i++;
          goto next;
        }
        case 's':
        {
          char * s = va_arg(arg, char *);
          if(!s)
            s = "(null)";
          PUTS(s);
          break;
        }
        case 'd':
        {
          int num = va_arg(arg, int);
          char nums[32];
          itoa(num, nums, 10); // haha num nums :-)
          PUTS(nums);
          break;
        }
        case 'x':
        {
          uint32_t x = va_arg(arg, uint32_t);
          char xs[32];
          itoa(x, xs, 16);
          if(HAS_FLAG(FLAG_FILL_ZERO))
          {
            for(; width > strlen(xs); width--)
            {
              str[str_i++] = HAS_FLAG(FLAG_FILL_ZERO) ? '0' : ' ';
              n++;
            }
            UNSET_FLAG(FLAG_FILL_ZERO);
            width = 0; /* reset width */
          }
          PUTS(xs);
          break;
        }
        case 'c':
        {
          char c = va_arg(arg, int);
          str[str_i++] = c;
          n++;
        }
        break;
      }
    }
    else
    {
      str[str_i++] = format[i];
      n++;
    }
  }
  str[str_i] = '\0';
  return n;
}

int vsprintf(char * str, const char format[], va_list arg)
{
  return vsnprintf(str, __INT_MAX__, format, arg);
}

int vprintf(const char format[], va_list arg)
{
  char buf[256];
  int ret = vsprintf(buf, format, arg);
  write(1, buf, 256);
  return ret;
}

int snprintf(char * str, size_t len, const char fmt[], ...)
{
  int rc;
  va_list arg;
  va_start(arg, fmt);
  rc = vsnprintf(str, len, fmt, arg);
  va_end(arg);
  return rc;
}

int sprintf(char * str, const char fmt[], ...)
{
  int rc;
  va_list arg;
  va_start(arg, fmt);
  rc = vsprintf(str, fmt, arg);
  va_end(arg);
  return rc;
}

int printf(const char * fmt, ...) {
  va_list arg;
  va_start(arg, fmt);
  int rc = vprintf(fmt, arg);
  va_end(arg);
  return rc;
}

void reverse(char * str[], int length)
{
  int start = 0;
  int end = length - 1;
  while (start < end)
  {
    char * temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    end--;
    start++;
  }
}

char * itoa(int value, char * str, int radix)
{
  char * rc;
  char * ptr;
  char * low;
  // Check for supported radix.
  if ( radix < 2 || radix > 36 )
  {
    *str = '\0';
    return str;
  }
  rc = ptr = str;
  // Set '-' for negative decimals.
  if ( value < 0 && radix == 10 )
  {
    *ptr++ = '-';
  }
  // Remember where the numbers start.
  low = ptr;
  // The actual conversion.
  do
  {
    // Modulo is negative for negative value. This trick makes abs() unnecessary.
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % radix];
    value /= radix;
  } while ( value );
  // Terminating the string.
  *ptr-- = '\0';
  // Invert the numbers.
  while ( low < ptr )
  {
    char tmp = *low;
    *low++ = *ptr;
    *ptr-- = tmp;
  }
  return rc;
}
