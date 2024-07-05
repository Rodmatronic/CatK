#ifndef __CTYPE_H
#define __CTYPE_H

#include <lib/common.h>

static inline bool isdigit(char c)
{
  if (c >= '0' && c <= '9')
    return true;
  return false;
}

static inline bool isspace(char c)
{
  return (c == ' ');
}

static inline int index_of(char c, char * str)
{
	int i = 0;
	while(str[i] != '\0')
	{
		if(str[i] == c)
			return i;
		i++;
	}
	return strlen(str);
}

#endif
