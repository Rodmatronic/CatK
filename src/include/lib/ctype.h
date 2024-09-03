#ifndef __CTYPE_H
#define __CTYPE_H

#include <lib/common.h>

static inline int isdigit(char c)
{
  return (c >= '0' && c <= '9');
}

static inline int isspace(char c)
{
  return (c == ' ');
}

static inline int islower(char c) {
	return ((c >= 'a') && (c <= 'z'));
}

static inline int isupper(char c) {
	return ((c >= 'A') && (c <= 'Z'));
}

static inline int isalpha(char c) {
  return (islower(c) || isupper(c));
}

static inline int index_of(char c, char * str)
{
	char i = 0;
	while(str[i] != '\0')
	{
		if(str[i] == c)
			return i;
		i++;
	}
	return strlen(str);
}

#endif
