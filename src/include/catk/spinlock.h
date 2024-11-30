#ifndef __SPINLOCK_H
#define __SPINLOCK_H

#include <stdatomic.h>
#include <catk/task.h>

struct spinlock
{
  atomic_flag value;
};

#define SPINLOCK_INIT(name) \
  struct spinlock name = {.value = ATOMIC_FLAG_INIT}

void spinlock_acquire(struct spinlock * s);
void spinlock_release(struct spinlock * s);

#endif
