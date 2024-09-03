#include <lib/common.h>

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
