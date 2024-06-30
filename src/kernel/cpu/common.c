#include <catk/core.h>

inline void critical_enter(void)
{
  asm volatile("cli");
}

inline void critical_exit(void)
{
  asm volatile("sti");
}
