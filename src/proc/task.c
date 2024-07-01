#include <catk/task.h>
#include <catk/compiler.h>
#include <catk/printk.h>
#include <catk/mem.h>
#include <catk/errno.h>
#include <catk/kernel.h>
#include <catk/debug.h>
#include <catk/core.h>
#include <lib/common.h>
#include <catk/types.h>

struct task * current;
struct task * catk_idle_task;

static bool is_tasking_enabled = false;

static inline void task_release(struct task * p);

void catk_idle(void)
{
  is_tasking_enabled = true;
  bootstrap2();
  for(;;);
}

bool tasking_enabled(void)
{
  return is_tasking_enabled;
}

inline struct task * get_current_task(void)
{
	return current;
}

void print_tasks(void)
{
  struct task * p = current;
  printk("[%c] \"%s\", pid %d, state %d, priority %d, ticks left until preemption %d\n", 
         (catk_idle_task == current) ? '*' : 0, catk_idle_task->name, catk_idle_task->pid, catk_idle_task->state, catk_idle_task->priority, catk_idle_task->ticks_left);
  while(p != catk_idle_task)
  {
    printk("[%c] \"%s\", pid %d, state %d, priority %d, ticks left until preemption %d\n", 
           (p == current) ? '*' : 0, p->name, p->pid, p->state, p->priority, p->ticks_left);
    p = p->next;
  }
}

static pid_t get_free_pid()
{
	struct task * p = catk_idle_task;
	for(pid_t pid = 0; pid < NPROC; pid++)
	{
		if (p->pid != pid)
		{
			return pid;
		}
		p = p->next;
	}
	return -1;
}

static struct task * get_proc_from_pid(pid_t pid)
{
	struct task * current = current;
	while (current != catk_idle_task)
	{
		if (current->pid == pid)
			return current;
		current = current->next;
	}
	return NULL;
}

int is_pid_running(pid_t pid)
{
	struct task * p = catk_idle_task;
	struct task * orig = catk_idle_task;
	while(1)
	{
		if(p->pid == pid)
		{
			return 1;
		}
		p = p->next;
		if(p == orig) 
			break;
	}
	return 0;
}

void kill(pid_t pid)
{
  struct task * p = get_proc_from_pid(pid);
	if (!p)
  {
    debug("[tasking] invalid pid %d\n", pid);
	  return;
  }
	debug("[tasking] killing \"%s\"\n", p->name);
  if(p->pid == 0)
    panic("CatK idle task killed!\n");
	is_tasking_enabled = false;
  task_release(p);
	p->prev->next = p->next;
	p->next->prev = p->prev;
	p->state = TASK_DEAD;
	is_tasking_enabled = true;
}

#define STACK_PUSH(item) \
  *(--stack) = (uint32_t)(item)

static inline void task_release(struct task * p)
{
  if(p)
  {
    if((void *)p->stack_top)
    {
      free((void *)p->stack_top);
    }
    free(p);
  }
}

// i guess vims find and replace feature did this lol
static struct task * create_kernel_task(char * name, uint32_t addr, int priority)
{
	struct task * p = (struct task *)calloc(sizeof(struct task), 1);
  if(!p)
    return NULL;
	p->name = name;
	p->pid = get_free_pid();
	p->state = TASK_CREATED;
  p->priority = priority;
  switch(p->priority)
  {
    case TASK_PRIORITY_HIGH:
    {
      p->time_quantum = 10;
      break;
    }
    case TASK_PRIORITY_NORMAL:
    {
      p->time_quantum = 5;
      break;
    }
    case TASK_PRIORITY_LOW:
    {
      p->time_quantum = 1;
      break;
    }
  }
  p->ticks_left = p->time_quantum;
  /* allocate stack for task */
	p->esp = (uint32_t)calloc(4096, 1);
  if(!(void *)p->esp)
  {
    free(p);
    return NULL;
  }
  p->stack_top = (p->esp + 4096);
	uint32_t * stack = (uint32_t *)p->stack_top;
	STACK_PUSH(0x00000202);
  STACK_PUSH(0x08);
	STACK_PUSH(addr);
	STACK_PUSH(0);
	STACK_PUSH(0);
	STACK_PUSH(0);
	STACK_PUSH(0);
	STACK_PUSH(0);
	STACK_PUSH(0);
	STACK_PUSH(p->stack_top);
	STACK_PUSH(0x10);
	STACK_PUSH(0x10);
	STACK_PUSH(0x10);
	STACK_PUSH(0x10);
	p->esp = (uint32_t)stack;
	return p;
}

int spawn_kernel_task(char * name, uint32_t addr, int priority)
{
  struct task * p = create_kernel_task(name, addr, priority);
  if(!p)
    return -ENOMEM;
  task_add_queue(p);
  return p->pid;
}

#undef STACK_PUSH

pid_t task_add_queue(struct task * p)
{
	is_tasking_enabled = false;
	p->next = current->next;
	p->next->prev = p;
	p->prev = current;
	current->next = p;
	is_tasking_enabled = true;
  debug("[tasking] added pid %d to queue\n", p->pid);
	return p->pid;
}

static void exec_task(void)
{
	current->state = TASK_ALIVE;
	asm volatile("mov %%eax, %%esp" : : "a"(current->esp));
	asm volatile("pop %gs");
	asm volatile("pop %es");
	asm volatile("pop %fs");
	asm volatile("pop %ds");
	asm volatile("pop %ebp");
	asm volatile("pop %edi");
	asm volatile("pop %esi");
	asm volatile("pop %edx");
	asm volatile("pop %ecx");
	asm volatile("pop %ebx");
	asm volatile("pop %eax");
	asm volatile("iret");
}

static struct task * find_next_task(void)
{
  struct task * p = current->next;
  while(1)
  {
    if(p->state == TASK_ALIVE || p->state == TASK_CREATED)
    {
      return p;
    }
    p = p->next;
  }
}

void schedule(void)
{
  critical_enter();
	asm volatile("push %eax");
	asm volatile("push %ebx");
	asm volatile("push %ecx");
	asm volatile("push %edx");
	asm volatile("push %esi");
	asm volatile("push %edi");
	asm volatile("push %ebp");
	asm volatile("push %ds");
	asm volatile("push %fs");
	asm volatile("push %es");
	asm volatile("push %gs");
  asm volatile("push %ss");
	asm volatile("mov %%esp, %%eax" : "=a"(current->esp));
  current = find_next_task();
	if (current->state == TASK_CREATED)
	{
		exec_task();
	}
	asm volatile("mov %%eax, %%esp" : : "a"(current->esp));
  asm volatile("pop %ss");
	asm volatile("pop %gs");
	asm volatile("pop %es");
	asm volatile("pop %fs");
	asm volatile("pop %ds");
	asm volatile("pop %ebp");
	asm volatile("pop %edi");
	asm volatile("pop %esi");
	asm volatile("pop %edx");
	asm volatile("pop %ecx");
	asm volatile("pop %ebx");
	asm volatile("pop %eax");
  critical_exit();
}

void tasking_init(void)
{
	catk_idle_task = create_kernel_task("catk-idle", (uint32_t)catk_idle, TASK_PRIORITY_HIGH);
	catk_idle_task->next = catk_idle_task;
	catk_idle_task->prev = catk_idle_task;
	current = catk_idle_task;
	exec_task();
  unreachable;
}
