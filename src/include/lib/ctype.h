#ifndef __CTYPE_H
#define __CTYPE_H

#include <lib/common.h>

static inline bool isdigit(char c)
{
  if (c >= '0' && c <= '9')
    return true;
  return false;
}

static inline int index_of(char c, char * str)
{
	for(int i = 0; str[i]; i++)
	{
		if(str[i] == c)
			return i;
	}
	return strlen(str);
}

#endif
