#include <catk/core.h>
#include <catk/printk.h>
#include <catk/console.h>
#include <catk/debug.h>
#include <catk/task.h>
#include <lib/common.h>

static char exceptions[32][32] = {
  "Divide Error",
  "Debug",
  "NMI Interrupt",
  "Breakpoint",
  "Overflow",
  "BOUND Range Exceeded",
  "Invalid Opcode",
  "Device Not Available",
  "Double Fault",
  "Coprocessor Segment Overrun",
  "Invalid TSS",
  "Segment Not Present",
  "Stack Segment Fault", 
  "General Protection Fault", 
  "Page Fault",
  "Reserved",
  "Floating-Point Error",
  "Alignment Check",
  "Machine Check",
  "SIMD Floating-Point Exception",
  "Virtualization Exception",
  "Control Protection Exception",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved"
};

intr_handler int_handlers[256] = {NULL};

#define CALL_INT_HANDLER \
  intr_handler handler = int_handlers[frame->intr]; \
  handler(frame);

void interrupt_install(void (*handler)(struct intr_stack_frame *), int intr)
{
  if(!handler)
    return;
  if(int_handlers[intr])
  {
    printk("interrupt_install: new interrupt handler conflicting with previous handler: 0x%02x\n", intr);
    return;
  }
  int_handlers[intr] = handler;
}

static int exception_handler(struct intr_stack_frame * frame)
{
  if(int_handlers[frame->intr])
  {
    CALL_INT_HANDLER;
    goto exit;
  }
  return -1;
exit:
  return 0;
}

static inline void register_dump(struct intr_stack_frame * reg)
{
  debug(" [+] error code: 0x%08x\n", reg->err_code);
  debug(" [+] eax: 0x%08x, ebx: 0x%08x, ecx: 0x%08x, edx: 0x%08x\n", reg->eax, reg->ebx, reg->ecx, reg->edx);
  debug(" [+] edi: 0x%08x, esi: 0x%08x, ebp: 0x%08x, esp: 0x%08x\n", reg->edi, reg->esi, reg->ebp, reg->esp);
  debug(" [+] eip: 0x%08x, cs: 0x%04x, ss: 0x%04x, eflags: 0x%08x, useresp: 0x%08x\n", reg->eip, reg->ss, reg->eflags, reg->useresp);
}

void interrupt_handler(struct intr_stack_frame * frame)
{
  if(frame->intr < 32)
  {
    if (exception_handler(frame) < 0)
    {
      if(is_console_enabled())
      {
        printk("Fatal Exception: 0x%02x\n", frame->intr);
        debug("[exception] received exception 0x%02x\n", frame->intr);
        register_dump(frame);
        critical_enter();
        for(;;);
      }
    }
  }
  else
  {
    if(int_handlers[frame->intr])
    {
      CALL_INT_HANDLER;
    }
    pic_eoi(frame->intr);
  }
}
