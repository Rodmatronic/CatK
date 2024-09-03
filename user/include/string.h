#ifndef __CATK_STRING_H
#define __CATK_STRING_H

#include "types.h"

#define NULL ((void *)0)

size_t strcspn(const char * s1, register const char * s2);
char * strtok(char * str, const char * delimiters);
int strcmp(const char *s1, const char *s2);
size_t strlen(const char * str);

#endif
