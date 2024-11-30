#include <catk/task.h>
#include <catk/printk.h>
#include <catk/mem.h>
#include <catk/core.h>
#include <catk/debug.h>
#include <catk/errno.h>
#include <catk/ipc.h>
#include <catk/kernel.h>
#include <catk/vfs.h>
#include <catk/virt.h>
#include <lib/common.h>
#include <stdint.h>

static struct task * task_list[NPROC];
static struct task * wait_queue[NPROC];
static struct task * current = NULL;

static size_t num_tasks = 0;

static bool tasking_enabled = false;

inline bool is_tasking_enabled(void) {
  return tasking_enabled;
}

struct task * get_task_from_pid(pid_t pid)
{
  for(int i = 0; i < NPROC; i++) {
    if(task_list[i] == NULL) {
      continue;
    }
    if(task_list[i]->pid == pid)  {
      return task_list[i];
    }
  }
  return NULL;
}

int is_pid_running(pid_t pid)
{
  struct task * p = get_task_from_pid(pid);
  return (p != NULL);
}

struct task * task_find_child(pid_t parent) {
  for(int i = 0; i < NPROC; i++) {
    if(task_list[i]->ppid == parent) {
      return task_list[i];
    }
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
  static pid_t pids = 0;
  pid_t alloc_pid = pids++;
  return (alloc_pid %= MAX_PID);
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
    case SIGSEGV: {
      debug("%s: received SIGSEGV!\n", current->name);
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

struct task * create_user_task(char * name, void * entry, uint8_t task_priority, int _unused_ argc, char _unused_ * argv[]) {
  /* since this starts off as a kernel task, i wont change utask to utask  */
  struct task * utask = (struct task *)malloc(sizeof(struct task));
  assert(utask != NULL);
  utask->name = name;
  utask->pid = pid_alloc();
  /* This should never happen */
  assert(utask->pid > 0);
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
  /* currently it isnt page aligned */
  utask->pgd = create_new_pgd();
  assert((uint32_t *)utask->pgd != NULL);
  utask->ticks_left = utask->time_quantum;
  utask->stack = (uintptr_t)malloc(4096);
  if(!utask->stack) {
    free(utask);
    free((uintptr_t *)utask->pgd);
    return NULL;
  }
  utask->error_code = -1;
  utask->fd[0] = (struct file *)malloc(sizeof(struct file));
  if(utask->fd[0] == NULL) {
    free(utask);
    free((uintptr_t *)utask->pgd);
    return NULL;
  }
  int rc = vfs_open(utask->fd[0], "/dev/tty0");
  if(IS_ERR(rc)) {
    free(utask->fd[0]);
    free(utask);
    free((uintptr_t *)utask->pgd);
    return NULL;
  }
  utask->fd[1] = utask->fd[0];
  utask->fd[2] = utask->fd[1];
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
  utask->regs.ebp = 0;
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
  switch (task_priority)
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
  ktask->pgd = get_kernel_pgd();
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
  ktask->regs.ebp = 0; /* <-- helps with stack unwinding/tracing */
  ktask->regs.esi = 0;
  ktask->regs.edi = 0;
  ktask->regs.ds = 0x0010;
  ktask->regs.cs = 0x0008;
  ktask->regs.useresp = ktask->regs.esp;
  memcpy((void *)ktask->regs.esp, &ktask->regs, sizeof(struct intr_stack_frame));
  debug("Created kernel-mode task with PID %d starting at 0x%08x\n", ktask->pid, ktask->regs.eip);
  return ktask;
}

void task_queue_sort(void) {
  struct task * current;
  for(int i = 0; i < NPROC; i++) {
    current = task_list[i];
    if(current == NULL) {
      int j;
      for(j = i; j < NPROC && task_list[j] == NULL; j++);
      current = task_list[j];
    } else {
      continue;
    }
  }
}

void task_add_queue(struct task * p)
{
  for(int i = 0; i < NPROC; i++) {
    if(task_list[i] == NULL) {
      task_list[i] = p;
      debug("scheduler: added pid %d to queue in slot %d\n", p->pid, i);
      num_tasks++;
      if(num_tasks == 1) {
        current = p;
      }
      return;
    }
  }
  panic("Could not add pid %d to task queue!\n", p->pid);
}

void task_remove_queue(struct task * p) {
  if(p == NULL) {
    return;
  }
  tasking_enabled = false;
  /* skip over the 'p' task */
  for(int i = 0; i < NPROC; i++) {
    if(p == task_list[i]) {
      task_list[i] = NULL;
    }
  }
  /* just in case if it isn't already */
  task_queue_sort();
  tasking_enabled = true;
  num_tasks--;
}

int task_get_task_queue_index(struct task * p) {
  for(int i = 0; i < NPROC; i++) {
    if(task_list[i] == p) {
      return i;
    }
  }
  return -1;
}

int spawn_kernel_task(char * name, void * addr, int priority)
{
  struct task * p = create_kernel_task(name, addr, priority);
  if (!p)
    return -EAGAIN;
  task_add_queue(p);
  return p->pid;
}

int spawn_user_task(char * name, void * addr, int priority, int argc, char * argv[]) {
  struct task * p = create_user_task(name, addr, priority, argc, argv);
  if (!p)
    return -EAGAIN;
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
  unreachable;
}

inline struct task * get_current_task(void) {
  return current;
}

static struct task * find_next_task(void)
{
  struct task * p;
  for(int i = 0; i < NPROC; i++)
  {
    if(task_list[i] == NULL) {
      i++;
      continue;
    }
    assert(task_list[i] != NULL);
    p = task_list[i];
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
  }
  if(num_tasks == 1) {
    return current;
  }
  debug("Number of tasks: %d\n", num_tasks);
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
  if(current->regs.esp <= current->stack) {
    panic("No stack memory left for pid %d!\n", current->pid);
    unreachable;
  }
  if(next->state == TASK_CREATED) {
    first_context_switch(&next->regs);
  }
  load_page_directory(next->pgd);
  memcpy(regs, &current->regs, sizeof(struct intr_stack_frame));
  critical_exit();
}

static void catk_sys_init(void) {
  tasking_enabled = true;
  debug("Multitasking has been enabled :)\n");
  bootstrap2();
  while(is_pid_running(1) == true);
  /* in this case, it hasnt really died, it hasnt been created yet */
  for(;;);
  unreachable;
}

void tasking_init(void) {
  /* using memset is bad practice */
  for(int i = 0; i < NPROC; i++) {
    task_list[i] = NULL;
    wait_queue[i] = NULL;
  }
  struct task * p = create_kernel_task("sys", catk_sys_init, TASK_PRIORITY_LOW);
  task_add_queue(p);
  first_context_switch(&p->regs);
}
