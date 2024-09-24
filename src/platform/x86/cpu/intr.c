#include <stdint.h>
#include <catk/platform.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <lib/common.h>

#include "segm.h"
#include "i8259.h"
#include "irq.h"

typedef void (*handler)(struct intr_stack_frame *);
/* im not quite sure how to do this without a typedef */
handler intr_handler_list[256] = {NULL};

static const char * trapnr_to_str(uint8_t trap) {
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

int intr_add_handler(int vector, void (*intr)(struct intr_stack_frame *)) {
  intr_handler_list[vector] = intr;
  return 0;
}

static inline int call_handler(struct intr_stack_frame * regs) {
  if(!intr_handler_list[regs->trapnr]) {
    return -1;
  }
  intr_handler_list[regs->trapnr](regs);
  return 0;
}

static void display_register_dump(struct intr_stack_frame * regs) {
  printk("CPU dump:\n");
  printk("\tEAX: 0x%08x ECX: 0x%08x EDX: 0x%08x EBX: 0x%08x\n", 
         regs->eax, regs->ecx, regs->edx, regs->ebx);
  printk("\tESP: 0x%08x EBP: 0x%08x ESI: 0x%08x EDI: 0x%08x\n", 
         regs->tmp, regs->ebp, regs->esi, regs->edi);
  printk("\tEIP: 0x%08x CS: 0x%04x EFLAGS: 0x%08x DS: 0x%04x\n", 
         regs->eip, regs->cs, regs->eflags, regs->ds);
  printk("Dumping code segment..\n");
  segment_dump(regs->cs);
  printk("Dumping data segment..\n");
  segment_dump(regs->ds);
}

void intr_handler(struct intr_stack_frame * regs) {
  if(regs->trapnr < 32) {
    debug("FATAL INTERRUPT %d\n", regs->trapnr);
    /* this is a trap interrupt, so we gotta end off with an IRETL / IRETD */
    int rc = call_handler(regs);
    if(rc < 0) {
      display_register_dump(regs);
      panic("Unhandled exception 0x%02x (%s)\n", regs->trapnr, trapnr_to_str(regs->trapnr));
    }
  } else {
    call_handler(regs);
    i8259_send_eoi(regs->trapnr);
  }
}
