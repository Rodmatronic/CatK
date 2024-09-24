#ifndef __TASK_H
#define __TASK_H

#include <catk/types.h>
#include <catk/limits.h>
#include <lib/common.h>
#include <stdint.h>

/* allowed number of processes */
#define MAX_PROCESSES 32

enum task_state {
  TASK_UNKNOWN, // 0
  TASK_CREATED, // 1
  TASK_ALIVE,   // 2
  TASK_BLOCKED, // 3
  TASK_ZOMBIE,  // 4
  TASK_DEAD     // 5
};

enum task_priority {
  TASK_PRIORITY_UNKNOWN,
  TASK_PRIORITY_HIGH,
  TASK_PRIORITY_NORMAL,
  TASK_PRIORITY_LOW
};

struct kthread {
  char name[NAME_MAX];
  pid_t pid;
  //struct intr_stack_frame regs;
  uintptr_t stack_mem;
};

#endif
