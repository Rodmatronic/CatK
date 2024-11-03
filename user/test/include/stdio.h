#ifndef __STDIO_H
#define __STDIO_H

int printf(const char * fmt, ...);
int strlen(char * str);
char * itoa(int value, char * str, int radix);
void reverse(char * str[], int length);
void * memset(void * dest, char val, int count);
int strcmp(const char * a, const char * b);

#endif
