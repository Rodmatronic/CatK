#include <catk/core.h>
#include <catk/trace.h>
#include <catk/printk.h>
#include <catk/kernel.h>
#include <catk/elf.h>
#include <catk/compiler.h>
#include <config.h>
#include <stdint.h>

struct stack_frame {
  struct stack_frame * bp;
  uintptr_t ip;
};

extern struct kern_syms symlist[];

char * trace_ret_addr(uintptr_t ret_eip) {
  for(uint32_t i = 0; i < ret_eip; i++) {
    if(symlist[i].addr >= ret_eip) {
      return symlist[i - 1].name;
    }
  }
  return "???";
}

#ifdef CATK_STACK_TRACE

static char * trace_addr(uintptr_t * offset, uintptr_t ip) {
  if(ip > (uintptr_t)&kend) {
    return "???";
  }
  for(uint32_t i = 0; i < ip; i++) {
    if(symlist[i].addr >= ip) {
      *offset = ip - symlist[i - 1].addr;
      return symlist[i - 1].name;
    }
  }
  return "???";
}

void trace_stack(uint8_t frames)
{
  printk("Stack backtrace:\n");
  printk("\t\t\tEIP          EBP\n");
  struct stack_frame * stack;
  asm volatile("movl %%ebp, %0" : "=r"(stack));
  for(int i = 0; stack && i < frames; i++)
  {
    uint32_t offset;
    printk("\t#%d: 0x%08x : [0x%08x] ", i, stack->ip, stack->bp);
    if(stack->ip)
      printk("%s+0x%08x\n", trace_addr(&offset, stack->ip), offset);
    else
      printk("\n");
    if((uintptr_t)stack->bp % sizeof(uintptr_t) != 0) {
      /* align the stack frame */
      stack = (struct stack_frame *)((uintptr_t)stack->bp + sizeof(uintptr_t) - (uintptr_t)stack->bp % sizeof(uintptr_t));
      break;
    }
    stack = stack->bp;
  }
  printk("End of stack backtrace.\n");
}

#endif
