#include <catk/platform.h>
#include <lib/common.h>

int rand(int radix)
{
  return gen_random() % radix;
}
