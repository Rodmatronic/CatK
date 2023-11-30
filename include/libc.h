#ifndef LIBC_H
#define LIBC_H

#include "types.h"
#include "../src/bin/libc.c"

char* k_strstr(const char* haystack, const char* needle);

char* k_strchr(const char* str, int c);

char* k_strcmp(const char* s1, const char* s2);

char* k_strtok(char* str, const char* delim);

char* k_strncmp(const char* str1, const char* str2, size n);

size strnlen(const char* str, size max_len);

char* strncat(char* dest, const char* src, size n);

int strncmp(const char* str1, const char* str2, size n);

int k_malloc(size size);

char* k_strrchr(const char* str, char c);

#endif
