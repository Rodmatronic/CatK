#include <catk/compiler.h>
#include <catk/debug.h>
/*
struct kthread * kthread_list[MAX_PROCESSES];

int sched_create_kthread(const char * name, uintptr_t func) {
  struct kthread * t = malloc(sizeof(struct kthread));
  uintptr_t * stack = calloc(4096, 1);
  memset(t, 0, sizeof(struct kthread));
  if(!t || !stack) {
    return -ENOMEM;
  }
  strncpy(t->name, name, NAME_MAX - 1);
  t->regs.eip = func;
  t->regs.eflags = 0x200LU;
  t->regs.cs = 0x0008;
  t->regs.ds = 0x0010;
  t->stack_mem = (uintptr_t)stack;
  t->regs.esp = (uintptr_t)(stack + 4096);
  t->regs.temp = t->regs.esp;
  t->regs.ebp = t->regs.esp;
  uintptr_t * stack_top = (uintptr_t *)t->regs.esp;
  stack_top-- = 
  debug("Created kernel thread %s\n", name);
}

void test_thread(void) {
  printk("I AM A TEST THREAD!");
  for(;;);
}

void sched_init(void) {
  memset(kthread_list, 0, sizeof(struct ktread) * MAX_PROCESSES);
  sched_create_kthread("test_thread", (uintptr_t)test_thread);
}*/
