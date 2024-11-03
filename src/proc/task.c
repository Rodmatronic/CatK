#include <catk/task.h>
#include <catk/printk.h>
#include <catk/mem.h>
#include <catk/core.h>
#include <catk/debug.h>
#include <catk/errno.h>
#include <catk/ipc.h>
#include <catk/kernel.h>
#include <catk/vfs.h>
#include <lib/common.h>
#include <lib/list.h>
#include <stdint.h>

static struct task * wait_queue[4]; // only used for sys_wait
static struct task * catk_sys;
static struct task * task_list;
static struct task * current;

static bool tasking_enabled = false;

inline bool is_tasking_enabled(void) {
  return tasking_enabled;
}

int is_pid_running(pid_t pid);

static void catk_sys_init(void) {
  tasking_enabled = true;
  debug("Multitasking has been enabled :)\n");
  bootstrap2();
  while(1);
}

struct task * get_task_from_pid(pid_t pid)
{
  struct task * p = catk_sys;
  struct task * orig = catk_sys;
  while (1)
  {
    if (p->pid == pid)
    {
      return p;
    }
    p = p->next;
    if (p == orig)
      break;
  }
  return NULL;
}

int is_pid_running(pid_t pid)
{
  struct task * p = get_task_from_pid(pid);
  return (p != NULL);
}

struct task * task_find_child(pid_t parent) {
  struct task * p = current;
  while(p != catk_sys) {
    if(p->ppid == current->pid) {
      return p;
    }
    p = p->next;
  }
  return NULL;
}

bool task_has_children(void) {
  return (task_find_child(current->pid) != NULL);
}

static int wait_queue_add(struct task * p) {
  for(int i = 0; i < 4; i++) {
    if (wait_queue[i] == NULL) {
      wait_queue[i] = p;
      return 0;
    }
  }
  return -1;
}

static void wait_queue_remove(struct task * p) {
  for(int i = 0; i < 4; i++) {
    if (wait_queue[i]->pid == p->pid) {
      wait_queue[i] = NULL;
      return;
    }
  }
}

// get most recent item in the wait queue
struct task * wait_queue_get_first(void) {
  for(int i = 0; i < 4; i++) {
    if(wait_queue[i] != NULL) {
      return wait_queue[i];
    }
  }
  return NULL;
}

pid_t sleep(void) {
  if(current->pid != 0) {
    critical_enter();
    debug("Sending PID %d off to a deep sleep. Goodnight! :)\n", current->pid);
    if (wait_queue_add(current) < 0) {
      return -EAGAIN;
    }
    current->state = TASK_BLOCKED;
    critical_exit();
    asm volatile("int $0x20");
    return current->pid;
  }
  return -EAGAIN;
}

void wakeup(pid_t pid) {
  if(pid == 0) {
    return;
  }
  struct task * p = get_task_from_pid(pid);
  assert(p == current);
  debug("Waking up PID %d (%s)...\n", pid, p->name);
  p->state = TASK_ALIVE;
  wait_queue_remove(p);
}

static inline pid_t pid_alloc(void)
{
  struct task *p = catk_sys;
  for (pid_t pid = 0; pid < NPROC; pid++)
  {
    if (p->pid != pid)
    {
      return pid;
   }
    p = p->next;
  }
  return -1;
}

static int task_signal(int signal) {
  switch(signal) {
    case SIGINT: {
      debug("%s: received SIGINT!\n", current->name);
      kill(current);
      break;
    }
    case SIGILL: {
      debug("%s: received SIGILL!\n", current->name);
      kill(current);
      break;
    }
    default: {
      debug("Who are you??\n");
      debug("\033[1;31mI am fizzbuzz..\033[1;0m\n");
      break;
    }
  }
  return 0;
}

struct task * create_user_task(char * name, void * entry, uint8_t task_priority, int argc, char * argv[]) {
  /* since this starts off as a kernel task, i wont change utask to utask  */
  struct task * utask = (struct task *)malloc(sizeof(struct task));
  if(!utask) {
    return NULL;
  }
  utask->name = name;
  utask->pid = pid_alloc();
  utask->uid = 0;
  utask->gid = 0;
  utask->ppid = 0;
  utask->state = TASK_CREATED;
  utask->priority = task_priority % 6;
  switch (utask->priority)
  {
    case TASK_PRIORITY_HIGH:
    {
      utask->time_quantum = 10;
      break;
    }
    case TASK_PRIORITY_NORMAL:
    {
      utask->time_quantum = 5;
      break;
    }
    case TASK_PRIORITY_LOW:
    {
      utask->time_quantum = 1;
      break;
    }
    default: {
      printk("Invalid task priority.\n");
      free(utask);
      return NULL;
    }
  }
  utask->ticks_left = utask->time_quantum;
  utask->stack = (uintptr_t)malloc(4096);
  if(!utask->stack) {
    free(utask);
    return NULL;
  }
  utask->error_code = -1;
  utask->argc = argc;
  vfs_open(utask->fd[0], "/dev/tty");
  utask->fd[1] = utask->fd[0];
  utask->fd[2] = utask->fd[1];
  for(int i = 0; i < argc; i++)
    utask->argv[i] = strdup(argv[i]);
  utask->cwd = NULL;
  utask->handle_signal = task_signal;
  /* set registers */
  utask->regs.ss = 0x23;
  utask->regs.eflags = 0x202;
  utask->regs.eip = (uintptr_t)entry;
  utask->regs.eax = 0;
  utask->regs.ecx = 0;
  utask->regs.edx = 0;
  utask->regs.ebx = 0;
  utask->regs.esi = 0;
  utask->regs.edi = 0;
  utask->regs.ds = 0x23;
  utask->regs.cs = 0x1b;
  utask->regs.esp = (utask->stack + 4096);
  utask->regs.esp -= sizeof(struct intr_stack_frame);
  utask->regs.ebp = utask->regs.esp;
  utask->regs.useresp = utask->regs.esp;
  memcpy((void *)utask->regs.esp, &utask->regs, sizeof(struct intr_stack_frame));
  debug("Created user-mode task with PID %d starting at 0x%08x\n", utask->pid, utask->regs.eip);
  return utask;
}

struct task * create_kernel_task(char * name, void * entry, uint8_t task_priority) {
  struct task * ktask = (struct task *)malloc(sizeof(struct task));
  if(!ktask) {
    return NULL;
  }
  ktask->name = name;
  ktask->pid = pid_alloc();
  ktask->uid = 0;
  ktask->gid = 0;
  ktask->ppid = 0;
  ktask->state = TASK_CREATED;
  ktask->priority = task_priority % 6;
  switch (ktask->priority)
  {
    case TASK_PRIORITY_HIGH:
    {
      ktask->time_quantum = 10;
      break;
    }
    case TASK_PRIORITY_NORMAL:
    {
      ktask->time_quantum = 5;
      break;
    }
    case TASK_PRIORITY_LOW:
    {
      ktask->time_quantum = 1;
      break;
    }
    default: {
      printk("Invalid task priority.\n");
      free(ktask);
      return NULL;
    }
  }
  ktask->ticks_left = ktask->time_quantum;
  ktask->stack = (uintptr_t)malloc(4096);
  if(!ktask->stack) {
    free(ktask);
    return NULL;
  }
  ktask->error_code = -1;
  ktask->argc = 0;
  ktask->argv = NULL;
  ktask->cwd = NULL;
  ktask->handle_signal = task_signal;
  /* set registers */
  ktask->regs.ss = 0x0010;
  ktask->regs.eflags = 0x202;
  ktask->regs.eip = (uintptr_t)entry;
  ktask->regs.eax = 0;
  ktask->regs.ecx = 0;
  ktask->regs.edx = 0;
  ktask->regs.ebx = 0;
  ktask->regs.esp = (ktask->stack + 4096);
  ktask->regs.esp -= sizeof(struct intr_stack_frame);
  ktask->regs.ebp = ktask->regs.esp;
  ktask->regs.esi = 0;
  ktask->regs.edi = 0;
  ktask->regs.ds = 0x0010;
  ktask->regs.cs = 0x0008;
  ktask->regs.useresp = ktask->regs.esp;
  memcpy((void *)ktask->regs.esp, &ktask->regs, sizeof(struct intr_stack_frame));
  debug("Created kernel-mode task with PID %d starting at 0x%08x\n", ktask->pid, ktask->regs.eip);
  return ktask;
}

pid_t task_add_queue(struct task * p)
{
  tasking_enabled = false;
  p->next = current->next;
  p->next->prev = p;
  p->prev = current;
  current->next = p;
  tasking_enabled = true;
  debug("scheduler: added pid %d to queue\n", p->pid);
  return p->pid;
}

void task_remove_queue(struct task * p) {
  /* skip over the 'p' task */
  p->prev->next = p->next;
  p->next->prev = p->prev;
}

int spawn_kernel_task(char * name, void * addr, int priority)
{
  struct task * p = create_kernel_task(name, addr, priority);
  if (!p)
    return -ENOMEM;
  task_add_queue(p);
  return p->pid;
}

int spawn_user_task(char * name, void * addr, int priority, int argc, char * argv[]) {
  struct task * p = create_user_task(name, addr, priority, argc, argv);
  if (!p)
    return -ENOMEM;
  task_add_queue(p);
  return p->pid;
}

void task_dealloc(struct task * task) {
  free((void *)task->stack);
  free(task);
}

void kill(struct task * p)
{
  if (!p)
    return;
  assert(p->pid >= 1);
  debug("Killing PID %d (%s)\n", p->pid, p->name);
  tasking_enabled = false;
  task_remove_queue(p);
  task_dealloc(p);
  tasking_enabled = true;
}

extern void do_first_context_switch(uintptr_t esp);

static void _noreturn_ first_context_switch(struct intr_stack_frame * regs) {
  current->state = TASK_ALIVE;
  do_first_context_switch((uint32_t)regs->esp);
  for(;;);
}

inline struct task * get_current_task(void) {
  return current;
}

static struct task * find_next_task(void)
{
  struct task * p = current->next;
  int i = 0; /* cant reach the amount of NPROC */
  for(; i < NPROC; i++)
  {
    switch(p->state) {
      case TASK_ALIVE: /* fall through */
      case TASK_CREATED: {
        return p;
      }
      case TASK_BLOCKED: {
        if(task_has_children() == true) {
          struct task * child = task_find_child(p->pid);
          if(child->state == TASK_DEAD) {
            p->state = TASK_ALIVE;
            return p;
          }
        }
      }
    }
    p = p->next;
  }
  panic("No free tasks left to schedule!\n");
  unreachable;
}

void schedule(struct intr_stack_frame * regs) {
  critical_enter();
  /* the task still has a slice of cpu time left */
  if(current->state != TASK_BLOCKED) {
    if(current->ticks_left >= 1) {
      current->ticks_left--;
      critical_exit();
      return;
    }
  }
  /* reset the time slice and switch to a new task */
  current->ticks_left = current->time_quantum;
  pic_eoi(0x20);
  /* save registers */
  memcpy(&current->regs, regs, sizeof(struct intr_stack_frame));
  struct task * next = find_next_task();
  current = next;
  /* no bad tasks, only good ones */
  if(current->regs.esp == current->stack) {
    panic("No stack memory left for pid %d!\n", current->pid);
    unreachable;
  }
  if(next->state == TASK_CREATED) {
    first_context_switch(&next->regs);
  }
  memcpy(regs, &current->regs, sizeof(struct intr_stack_frame));
  critical_exit();
}

void tasking_init(void) {
  /* clear the wait queue */
  memset(wait_queue, 0, sizeof(struct task) * 4);
  catk_sys = create_kernel_task("system", catk_sys_init, TASK_PRIORITY_HIGH);
  if(!catk_sys) {
    return;
  }
  task_list = catk_sys;
  task_list->next = catk_sys;
  task_list->prev = catk_sys;
  current = catk_sys;
  first_context_switch(&task_list->regs);
  unreachable;
}
