#include <catk/core.h>
#include <lib/common.h>

int rand(int range)
{
  return entropy() % range;
}