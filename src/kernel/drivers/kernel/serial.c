#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <term.h>
#include <pc.h>
#include "printk.h"
#include "serial.h"

int init_serial() {
   printk("init_serial: init\n");
   outportb(PORT + 1, 0x00);    // Disable all interrupts
   outportb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outportb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outportb(PORT + 1, 0x00);    //                  (hi byte)
   outportb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outportb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outportb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outportb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outportb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
 
   // Check if serial is faulty (i.e: not same byte as sent)
   if(inportb(PORT + 0) != 0xAE) {
      return 1;
   }
   printk("init_serial: not faulty. good\n");
 
   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   outportb(PORT + 4, 0x0F);
   printk("init_serial: loaded!\n");
   return 0;
}

int serial_received() {
   return inportb(PORT + 5) & 1;
}
 
char read_serial() {
   while (serial_received() == 0);
 
   return inportb(PORT);
}

int is_transmit_empty() {
   return inportb(PORT + 5) & 0x20;
}
 
void write_serial(char a) {
   while (is_transmit_empty() == 0);
 
   outportb(PORT,a);
}