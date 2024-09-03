#include <catk/spinlock.h>
#include <catk/platform.h>
#include <stdatomic.h>

void spinlock_acquire(struct spinlock * s)
{
  critical_enter();
  while(atomic_flag_test_and_set_explicit(&s->value, memory_order_acquire))
  {
    __builtin_ia32_pause();
  }
  critical_exit();
}

void spinlock_release(struct spinlock * s)
{
  critical_enter();
	atomic_flag_clear_explicit(&s->value, memory_order_release);
  critical_exit();
}
