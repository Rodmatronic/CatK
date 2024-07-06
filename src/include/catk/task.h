#ifndef __TASK_H
#define __TASK_H

#include <catk/types.h>
#include <catk/fs.h>
#include <catk/limits.h>
#include <lib/common.h>
#include <stdint.h>

/* allowed number of processes */
#define NPROC 32

enum task_state
{
  TASK_UNKNOWN, // 0
  TASK_CREATED, // 1
  TASK_ALIVE,   // 2
  TASK_BLOCKED, // 3
  TASK_ZOMBIE,  // 4
  TASK_DEAD     // 5
};

enum task_priority
{
  TASK_PRIORITY_UNKNOWN,
  TASK_PRIORITY_HIGH,
  TASK_PRIORITY_NORMAL,
  TASK_PRIORITY_LOW
};

struct task
{
	char * name;
  pid_t pid;
  uint8_t state;
  uint8_t priority;
  uint8_t time_quantum;
  uint8_t ticks_left;
	uint32_t stack_top; /* used only when freeing a task */
  uint32_t esp;
  char * cwd;
  struct file * fd[OPEN_MAX];
	struct task * next;
  struct task * prev;
};

#define WAIT_FOR_PID(pid) while(is_pid_running(pid));

void print_tasks(void);
void schedule(void);
int is_pid_running(pid_t pid);
void kill(struct task * p);
struct task * get_current_task(void);
bool tasking_enabled(void);
int spawn_kernel_task(char * name, uint32_t addr, int priority);
pid_t task_add_queue(struct task * p);
void tasking_init(void);

#endif
