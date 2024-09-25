#ifndef __TASK_H
#define __TASK_H

#include <catk/types.h>
#include <catk/fs.h>
#include <catk/core.h>
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
  pid_t ppid;
  uid_t uid;
  gid_t gid;
  uint8_t state;
  uint8_t priority;
  uint8_t time_quantum;
  uint8_t ticks_left;
	uint32_t stack_top; /* used only when freeing a task */
  uint32_t error_code;
  uint32_t esp;
  bool kernel_mode;
  char * cwd;
  int (*handle_signal)(int); /* each task can handle a signal differently */
  uint32_t entry;
  struct file * fd[OPEN_MAX];
	struct task * next;
  struct task * prev;
};

void print_tasks(void);
void schedule(void);
int is_pid_running(pid_t pid);
void kill(struct task * p);
struct task * get_current_task(void);
bool tasking_enabled(void);
int spawn_kernel_task(char * name, void * addr, int priority);
int spawn_user_task(char * name, void * addr, int priority);
struct task * create_kernel_task(char * name, void * addr, int priority);
struct task * create_user_task(char * name, void * addr, int priority);
pid_t task_add_queue(struct task * p);
void tasking_init(void);
bool task_has_children(void);
struct task * task_find_child(pid_t parent);
struct task * get_task_from_pid(pid_t pid);
pid_t sleep(void);

#endif
