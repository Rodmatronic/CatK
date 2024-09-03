#include <stdint.h>
#include <catk/platform.h>
#include <catk/printk.h>
#include <lib/common.h>

#include "irq.h"

static inline char * gpf_tbltostr(const uint8_t tbl) {
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
  return NULL;
}

static void gpf_handler(struct intr_stack_frame * regs) {
  printk("x86 Trap: General Protection Fault:\n");
  /* bit 0 is set if it was caused by software */
  printk("\tExternal: %s\n", (regs->error & BIT(0)) ? "true" : "false");
  /* bits 1-2 are set to tell us what table it originated from */
  uint8_t table = ((regs->error >> 1) & 0xf0) & 0b1100;
  printk("\tTable: %s (0x%02x)\n", gpf_tbltostr(table), table);
  /* bit 3-15 is the selector index in the table */
  uint8_t idx = (regs->error >> 3);
  printk("\tSelector index of origin: %d\n", idx);
  /* bit 16-31 are zero-padded to form a uint32_t (we ignore those bits) */
}

void exceptions_install(void) {
  intr_add_handler(X86_TRAP_GP, gpf_handler);
}
