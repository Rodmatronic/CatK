#include <catk/debug.h>
#include <catk/compiler.h>
#include <catk/task.h>
#include <catk/trace.h>
#include <stdint.h>

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;
 
void _cold_ _noreturn_ __stack_chk_fail(void)
{
  debug("Stack smashing detected @ 0x%08x.\n", __builtin_return_address (0));
	die();
  unreachable;
}

void _cold_ _noreturn_ __stack_chk_fail_local(void)
{
  __stack_chk_fail();
  unreachable;
}
