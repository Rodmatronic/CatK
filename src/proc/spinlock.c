#include <catk/spinlock.h>
#include <catk/core.h>
#include <stdatomic.h>

void spinlock_acquire(struct spinlock * s)
{
  while(atomic_flag_test_and_set_explicit(&s->value, memory_order_acquire))
  {
    __builtin_ia32_pause();
  }
}

void spinlock_release(struct spinlock * s)
{
	atomic_flag_clear_explicit(&s->value, memory_order_release);
}
