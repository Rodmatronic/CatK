#include <catk/printk.h>
#include <catk/compiler.h>
#include <stdint.h>

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;
 
void _cold_ _noreturn_ __stack_chk_fail(void)
{
	panic("Stack smashing detected. Report this to the CatK GitHub repository at https://github.com/Rodmatronic/CatK/issues\n");
}

void _cold_ __stack_chk_fail_local(void)
{
  __stack_chk_fail();
  unreachable;
}
