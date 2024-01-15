#ifndef LIBC_H
#define LIBC_H

#include "types.h"

int sprintf(char* buffer, const char* format, uint32 value);

char* k_strstr(const char* haystack, const char* needle);

char* k_strchr(const char* str, int c);

int k_strcmp(const char* s1, const char* s2);

char* k_strtok(char* str, const char* delim);

char k_strncmp(const char* str1, const char* str2, size n);

size strnlen(const char* str, size max_len);

char* strncat(char* dest, const char* src, size n);

int strncmp(const char* str1, const char* str2, size n);

void* k_malloc(size size);

char* k_strrchr(const char* str, char c);

int snprintf(char *str, size size, const char *format, const char *arg, ...);

#endif
