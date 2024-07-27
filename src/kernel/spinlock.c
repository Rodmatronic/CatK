#include <catk/spinlock.h>
#include <catk/core.h>
#include <catk/debug.h>
#include <stdatomic.h>

void spinlock_acquire(struct spinlock * s)
{
  critical_enter();
  struct task * current = get_current_task();
  while( atomic_flag_test_and_set_explicit(&s->value, memory_order_acquire))
  {
    __builtin_ia32_pause();
  }
  if(current)
  {
    s->owner = current;
  }
  critical_exit();
}

void spinlock_release(struct spinlock * s)
{
  critical_enter();
	atomic_flag_clear_explicit(&s->value, memory_order_release);
  s->owner = NULL;
  critical_exit();
}
