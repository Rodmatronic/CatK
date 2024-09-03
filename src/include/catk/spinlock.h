#ifndef __SPINLOCK_H
#define __SPINLOCK_H

#include <stdatomic.h>
#include <catk/task.h>

struct spinlock
{
  atomic_flag value;
  struct task * owner;
};

#define SPINLOCK_INIT(name) \
  struct spinlock name = {ATOMIC_FLAG_INIT, NULL}

void spinlock_acquire(struct spinlock * s);
void spinlock_release(struct spinlock * s);

#endif
