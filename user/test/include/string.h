#ifndef __CATK_STRING_H
#define __CATK_STRING_H

#define NULL ((void *)0)

int strcspn(const char * s1, register const char * s2);
char * strtok(char * str, const char * delimiters);
int strcmp(const char *s1, const char *s2);
int strlen(const char * str);

#endif
