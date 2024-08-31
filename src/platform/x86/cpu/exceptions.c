#include <stdint.h>
#include <catk/platform.h>
#include <catk/printk.h>
#include <lib/common.h>

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
  printk("\tExternal: %s\n", (regs->error & BIT(0)) ? "true" : "false");
  uint8_t table = ((regs->error >> 1) & 0xf0) & 0b1100;
  printk("\tTable: %s (0x%02x)\n", gpf_tbltostr(table), table);
  uint8_t idx = (regs->error >> 3);
  printk("\tSelector index: %d\n", idx);
}

void exceptions_install(void) {
  intr_add_handler(0x0d, gpf_handler);
}
