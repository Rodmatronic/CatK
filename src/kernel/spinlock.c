#include <catk/spinlock.h>
#include <stdatomic.h>

void spinlock_acquire(struct spinlock * s)
{
  struct task * current = get_current_task();
  while( atomic_flag_test_and_set_explicit(&s->value, memory_order_acquire))
  {
    __builtin_ia32_pause();
  }
  if(current)
  {
    s->owner = current;
  }
}

void spinlock_release(struct spinlock * s)
{
	atomic_flag_clear_explicit(&s->value, memory_order_release);
  s->owner = NULL;
}
