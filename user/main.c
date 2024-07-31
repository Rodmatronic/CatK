#include <stdint.h>

extern int print(char * string);
extern int open(const char * path, int flags, uint16_t mode);

int main(void)
{
  int rc;
  print("Hello CatK\n");
  rc = open("hi", 0, 0);
  return rc;
}