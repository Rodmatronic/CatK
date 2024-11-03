#include <catk/core.h>
#include <catk/printk.h>
#include <catk/console.h>
#include <catk/debug.h>
#include <catk/task.h>
#include <catk/core.h>
#include <lib/common.h>

intr_handler int_handlers[255] = { NULL };

#define CALL_INT_HANDLER \
  intr_handler handler = int_handlers[frame->intr]; \
  handler(frame);

static const char * trap_to_str(uint8_t trap) {
  switch(trap) {
    case X86_TRAP_DE:
      return "Divide-by-zero"; 
    case X86_TRAP_DB:
      return "Debug";
    case X86_TRAP_NMI:
      return "Non-maskable Interrupt";
    case X86_TRAP_BP:
      return "Breakpoint";
    case X86_TRAP_OF:
      return "Overflow";
    case X86_TRAP_BR:
      return "Bound Range Exceeded";
    case X86_TRAP_UD:
      return "Invalid Opcode";
    case X86_TRAP_NM:
      return "Device Not Available";
    case X86_TRAP_DF:
      return "Double Fault";
    case X86_TRAP_TS:
      return "Invalid TSS";
    case X86_TRAP_NP:
      return "Segment Not Present";
    case X86_TRAP_SS:
      return "Stack Segment Fault";
    case X86_TRAP_GP:
      return "General Proteciton Fault";
    case X86_TRAP_PF:
      return "Page Fault";
    case X86_TRAP_SPURIOUS:
      return "Spurious Interrupt";
    case X86_TRAP_MF:
      return "x87 Floating-Point Exception";
    case X86_TRAP_AC:
      return "Alignment Check";
    case X86_TRAP_MC:
      return "Machine Check";
    case X86_TRAP_XF:
      return "SIMD Floating-Point Exception";
    case X86_TRAP_VE:
      return "Virtualization Exception";
    case X86_TRAP_CP:
      return "Control Protection Exception";
    case X86_TRAP_VC:
      return "VMM Communication Exception";
    default:
      return "Reserved";
  }
}

void interrupt_install(void (*handler)(struct intr_stack_frame *), uint8_t intr)
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
    return 0;
  }
  return -1;
}

static inline void register_dump(struct intr_stack_frame * reg)
{
  printk("CPU dump:\n");
  printk("\tEAX: 0x%08x ECX: 0x%08x EDX: 0x%08x EBX: 0x%08x\n", 
         reg->eax, reg->ecx, reg->edx, reg->ebx);
  printk("\tESP: 0x%08x EBP: 0x%08x ESI: 0x%08x EDI: 0x%08x\n", 
         reg->esp, reg->ebp, reg->esi, reg->edi);
  printk("\tEIP: 0x%08x CS: 0x%04x EFLAGS: 0x%08x DS: 0x%04x\n", 
         reg->eip, reg->cs, reg->eflags, reg->ds);

  debug("CPU dump:\n");
  debug("\tEAX: 0x%08x ECX: 0x%08x EDX: 0x%08x EBX: 0x%08x\n", 
         reg->eax, reg->ecx, reg->edx, reg->ebx);
  debug("\tESP: 0x%08x EBP: 0x%08x ESI: 0x%08x EDI: 0x%08x\n", 
         reg->esp, reg->ebp, reg->esi, reg->edi);
  debug("\tEIP: 0x%08x CS: 0x%04x EFLAGS: 0x%08x DS: 0x%04x\n", 
         reg->eip, reg->cs, reg->eflags, reg->ds);
  /*
  printk("Dumping code segment..\n");
  segment_dump(reg->cs);
  printk("Dumping data segment..\n");
  segment_dump(reg->ds);
  */
}

void interrupt_handler(struct intr_stack_frame * frame)
{
  /* this is a trap interrupt */
  if(frame->intr < 32)
  {
    if (exception_handler(frame) < 0)
    {
      if(is_console_enabled())
      {
        bool user_mode = (frame->cs == 0x1b);
        printk("Fatal trap %d: %s while in %s\n", frame->intr, trap_to_str(frame->intr), user_mode ? "user mode" : "kernel mode");
        register_dump(frame);
        if(user_mode == true) {
          kill(get_current_task());
        }
        panic("\n");
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
