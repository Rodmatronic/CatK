#include "types.h"
#include "string.h"

size_t strlen(const char * str) {
  int i;
  for(i = 0; str[i]; i++);
  return i;
}

size_t strcspn(const char * s1, register const char * s2)
{
  register const char *p, *spanp;
	register char c, sc;

	/*
	 * Stop as soon as we find any character from s2.  Note that there
	 * must be a NUL in s2; it suffices to stop when we find that, too.
	 */
	for (p = s1;;) {
		c = *p++;
		spanp = s2;
		do {
			if ((sc = *spanp++) == c)
				return (p - 1 - s1);
		} while (sc != 0);
	}
	/* NOTREACHED */
}

char * strtok(char * str, const char * delimiters)
{
  static char * nextToken = NULL;
  if (str != NULL)
  {
    nextToken = str;
  }
  if (nextToken == NULL)
  {
    return NULL;
  }
  char * tokenStart = nextToken;
  int tokenLength = strcspn(nextToken, delimiters);
  nextToken += tokenLength;
  if (*nextToken != '\0')
  {
    *nextToken = '\0';
    nextToken++;
  }
  else
  {
    nextToken = NULL;
  }
  return tokenStart;
}

int strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2++)
		if (*s1++ == '\0')
			return (0);
	return (*(const unsigned char *)s1 - *(const unsigned char *)(s2 - 1));
}
