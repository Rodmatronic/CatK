#ifndef __COMMON_H
#define __COMMON_H

#include <catk/types.h>
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
int strcmp(const char * str1, const char * str2);
int strncmp(const char * str1, const char * str2, size_t n);
void strcpy(char * dest, const char * src);
void substrr(int s, int e, char * src, char * dest);
char * strtok(char * str, const char * delimiters);
int strcspn(const char * str, const char * charset);
size_t strsplit(char * str, const char delim);

/* conversion stuff */
char * itoa(int value, char * str, int radix);

/* memory stuff */
void * memcpy(void * dest, const void * src, size_t count);
void * memset(void * dest, char val, size_t count);
void * memmove(void * dest, const void * src, size_t n);
int memcmp(const void * mem1, const void * mem2, size_t n);

/* printf and their siblings */
int vsnprintf(char * str, size_t len, const char format[], va_list arg);
int vsprintf(char * str, const char format[], va_list arg);
int vprintf(const char format[], va_list arg);
int sprintf(char * str, const char fmt[], ...);
int snprintf(char * str, size_t len, const char fmt[], ...);




#endif
