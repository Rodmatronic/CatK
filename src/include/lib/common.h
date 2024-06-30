#ifndef __COMMON_H
#define __COMMON_H

#include <sys/types.h>
#include <stdarg.h>

#define BIT(bit) (1 << bit)
#define ALIGN(align, num) (num + align - 1) & ~(align - 1)

typedef enum
{
  true = 1,
  false = 0
}bool;

#ifndef NULL
#define NULL ((void *)0)
#endif

/* string stuff */
void strncpy(char * dest, const char * src, size_t n);
size_t strlen(const char * str);
void reverse(char * str, size_t sz);
char * strdup(char * str);
char * strcat(char * dest, const char * src);

/* conversion stuff */
char * itoa(int value, char * str, int radix);


/* memory stuff */
void * memcpy(void * dest, const void * src, size_t count);
void * memset(void * dest, char val, size_t count);
void * memmove(void * dest, const void * src, size_t n);

/* printf and their siblings */
int vsnprintf(char * str, size_t len, const char * format, va_list arg);
int vsprintf(char * str, char * format, va_list arg);
int vprintf(char * format, va_list arg);
int sprintf(char * str, char * fmt, ...);
int snprintf(char * str, size_t len, char * fmt, ...);




#endif
