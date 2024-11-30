#include <catk/core.h>
#include <catk/compiler.h>
#include <catk/types.h>
#include <catk/fs.h>
#include <catk/errno.h>
#include <catk/spinlock.h>
#include <catk/printk.h>
#include <catk/limits.h>
#include <catk/io.h>
#include <lib/common.h>
#include <stdint.h>

/* general definitions and/or helper macros */

#define RS232_PORT0 0x3f8
#define RS232_PORT1 0x2f8
#define RS232_PORT2 0x3e8
#define RS232_PORT3 0x2e8
#define RS232_PORT4 0x5f8
#define RS232_PORT5 0x4f8
#define RS232_PORT6 0x5e8
#define RS232_PORT7 0x4e8
#define RS232_TOTAL 8

/* RS232 regsiters and related stuff */

/* Interrupt Enable Register (or IER for short) */
#define RS232_IER_OFFSET 1
/* FIFO Control Register */
#define RS232_FIFO_OFFSET 2
/* Line Control Register */
#define RS232_LCR_OFFSET 3
/* Modem Control Register */
#define RS232_MCR_OFFSET 4
/* Line Status Register */
#define RS232_LSR_OFFSET 5
/* Modem Status Register */
#define RS232_MSR_OFFSET 6
/* Scratch Register */
#define RS232_SR_OFFSET 7

#define RS232_DLAB_LSB 0
#define RS232_DLAB_MSB 1

/* Baud Rates */

#define RS232_BAUD_115200 1
#define RS232_BAUD_57600 2
#define RS232_BAUD_38400 3
#define RS232_BAUD_19200 4
#define RS232_BAUD_9600 5
#define RS232_BAUD_4800 6
#define RS232_BAUD_2400 7
#define RS232_BAUD_1200 8
#define RS232_BAUD_300 9
#define RS232_BAUD_110 10

/* Line Control Register configurable bits */
 
#define RS232_LCR_CHAR_LEN5 0b00
#define RS232_LCR_CHAR_LEN6 0b01
#define RS232_LCR_CHAR_LEN7 0b10
#define RS232_LCR_CHAR_LEN8 0b11

#define RS232_LCR_STOP BIT(2)

#define RS232_LCR_PARITY_NONE (0b000 << 3)
#define RS232_LCR_PARITY_ODD (0b001 << 3)
#define RS232_LCR_PARITY_EVEN (0b011 << 3)
#define RS232_LCR_PARITY_MARK (0b101 << 3)
#define RS232_LCR_PARITY_SPACE (0b111 << 3)

#define RS232_LCR_BRK_ENABLE BIT(6)
#define RS232_LCR_DLAB BIT(7)

struct rs232_interface {
  uint16_t io_port;
  uint8_t irq;
  struct spinlock lock;
  bool faulty;
};

struct rs232_interface rs232_ints[RS232_TOTAL];

void rs232_set_divisor(struct rs232_interface * interface, uint16_t divisor) {
  /* set dlab */
  outb(interface->io_port + RS232_LCR_OFFSET, inb(interface->io_port + RS232_LCR_OFFSET) | RS232_LCR_DLAB);
  /* send least, and most significant bytes */
  outb(interface->io_port + RS232_DLAB_LSB, (uint8_t)(divisor & 0xff));
  outb(interface->io_port + RS232_DLAB_MSB, (uint8_t)((divisor >> 8) & 0xff));
  /* we dont want to stay in DLAB mode. exit */
  outb(interface->io_port + RS232_LCR_OFFSET, inb(interface->io_port + RS232_LCR_OFFSET) & ~RS232_LCR_DLAB);
}

void rs232_set_char_length(struct rs232_interface * interface, uint8_t len) {
  /* clear previous character length */
  outb(interface->io_port + RS232_LCR_OFFSET, inb(interface->io_port + RS232_LCR_OFFSET) & ~3);
  outb(interface->io_port + RS232_LCR_OFFSET, inb(interface->io_port + RS232_LCR_OFFSET) | len);
  if(len == RS232_LCR_CHAR_LEN5) {
    outb(interface->io_port + RS232_LCR_OFFSET, inb(interface->io_port + RS232_LCR_OFFSET) | RS232_LCR_STOP);
  }
  outb(interface->io_port + RS232_LCR_OFFSET, inb(interface->io_port + RS232_LCR_OFFSET) & ~RS232_LCR_STOP);
}

void rs232_set_parity(struct rs232_interface * interface, uint8_t parity) {
  outb(interface->io_port + RS232_LCR_OFFSET, inb(interface->io_port + RS232_LCR_OFFSET) & ~(7 << 3));
  outb(interface->io_port + RS232_LCR_OFFSET, inb(interface->io_port + RS232_LCR_OFFSET) | parity);
}
