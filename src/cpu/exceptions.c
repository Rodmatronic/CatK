#include <catk/core.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/ipc.h>
#include <lib/common.h>
#include <config.h>
#include <stdint.h>

/* General Protection Fault */

static inline char * gpf_tbl2str(const uint8_t tbl) {
  switch(tbl) {
    case 0b00: {
      return "GDT";
    }
    case 0b01: {
      return "IDT";
    }
    case 0b10: {
      return "LDT";
    }
    case 0b11: {
      return "IDT";
    }
  }
  return "???";
}

static void gpf_handler(struct intr_stack_frame * regs) {
  bool external_err = (regs->err_code & BIT(0));
  uint8_t table_origin = ((regs->err_code >> 1) & 0b11);
  uint16_t table_index = (regs->err_code >> 3);
  bool user_mode = (regs->cs == 0x1b);
  printk("x86 Trap: General Protection Fault\n");
  if(regs->err_code == 0) {
    printk("IRETQ fault detected\n");
  } else {
    printk("External: %s\n", external_err ? "true" : "false");
    printk("Table of origin: %s\n", gpf_tbl2str(table_origin));
    printk("Table index: %d\n", table_index);
  }
  if(user_mode == true) {
    dispatch_signal(SIGILL);
    return;
  }
  panic("General Protection Fault in kernel mode!\n");
  unreachable;
}

void division_by_zero_handler(struct intr_stack_frame * regs) {
  if(regs->cs != 0x1b) {
    panic("Division by zero in kernel mode!\n");
    unreachable;
  }
  dispatch_signal(SIGFPE);
}

#ifdef CATK_DEBUG_BUILD
void breakpoint_handler(struct intr_stack_frame * reg) {
#else
void breakpoint_handler(struct intr_stack_frame _unused_ * reg) {
#endif
#ifdef CATK_DEBUG_BUILD
  critical_enter();
  debug("KERNEL BREAKPOINT HAS BEEN REACHED!\n");
  debug("EAX: 0x%08x ECX: 0x%08x EDX: 0x%08x EBX: 0x%08x\n", 
         reg->eax, reg->ecx, reg->edx, reg->ebx);
  debug("ESP: 0x%08x EBP: 0x%08x ESI: 0x%08x EDI: 0x%08x\n", 
         reg->esp, reg->ebp, reg->esi, reg->edi);
  debug("EIP: 0x%08x CS: 0x%04x EFLAGS: 0x%08x DS: 0x%04x\n", 
         reg->eip, reg->cs, reg->eflags, reg->ds);
  die();
  critical_exit();
#else
  dispatch_signal(SIGTRAP);
#endif
}

void exceptions_install(void) {
  interrupt_install(gpf_handler, X86_TRAP_GP);
  interrupt_install(division_by_zero_handler, X86_TRAP_DE);
  interrupt_install(breakpoint_handler, X86_TRAP_BP);
}
