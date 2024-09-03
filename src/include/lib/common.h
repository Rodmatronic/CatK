#ifndef __COMMON_H
#define __COMMON_H

#include <catk/types.h>
#include <stdarg.h>

#define BIT(bit) (1 << bit)
#define ZEROBIT(bit) (0 << bit)
#define ALIGN(align, num) ((num + align - 1) & ~(align - 1))

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
char * strdup(const char * str);
char * strcat(char * dest, const char * src);
int strcmp(const char * str1, const char * str2);
int strncmp(const char * str1, const char * str2, size_t n);
void strcpy(char * dest, const char * src);
void substrr(int s, int e, char * src, char * dest);
char * strtok(char * str, const char * delimiters);
int strcspn(const char * str, const char * charset);
size_t strsplit(char * str, const char delim);
char * strstr(const char * s1, const char * s2);
char * strchr(const char * str, int ch);

/* conversion stuff */
char * itoa(int value, char * str, int radix);
int atoi(const char * str);

/* memory stuff */
void * memcpy(void * dest, const void * src, size_t count);
void * memcpy16(void * dest, const void * src, size_t count);
void * memcpy32(void * dest, const void * src, size_t count);
void * memset(void * dest, char val, size_t count);
void * memset16(void * dest, uint16_t val, size_t count);
void * memset32(void * dest, uint32_t val, size_t count);
void * memmove(void * dest, const void * src, size_t n);
void * memmove16(void * dest, const void * src, size_t n);
void * memmove32(void * dest, const void * src, size_t n);
int memcmp(const void * addr1, const void * addr2, size_t count);
void hexdump(const void * ptr, size_t len);

/* printf and their siblings */
int vsnprintf(char * str, size_t len, const char format[], va_list arg);
int vsprintf(char * str, const char format[], va_list arg);
int vprintf(const char format[], va_list arg);
int sprintf(char * str, const char fmt[], ...);
int snprintf(char * str, size_t len, const char fmt[], ...);
/* random number generation */
int rand(int radix);

#endif
