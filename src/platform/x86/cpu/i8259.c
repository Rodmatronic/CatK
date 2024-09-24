#include <stdint.h>
#include <catk/io.h>
#include <lib/common.h>

#define PIC_MASTER_COMMAND_PORT   0x0020
#define PIC_MASTER_DATA_PORT      0x0021
#define PIC_SLAVE_COMMAND_PORT    0x00a0
#define PIC_SLAVE_DATA_PORT       0x00a1

#define PIC_COMMAND_END_OF_INTR   0x20

#define ICW1_ICW4	      0x01		    /* Indicates that ICW4 will be present */
#define ICW1_SINGLE	    0x02		    /* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04	      /* Call address interval 4 (8) */
#define ICW1_LEVEL	    0x08		    /* Level triggered (edge) mode */
#define ICW1_INIT	      0x10		    /* Initialization */

#define ICW4_8086	      0x01		    /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	      0x02		    /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08	      /* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C	      /* Buffered mode/master */
#define ICW4_SFNM	      0x10		    /* Special fully nested (not) */

static void i8259_unmask(uint8_t intr)
{
  uint16_t port;
  uint8_t value;
 
  if(intr < 8)
  {
    port = PIC_MASTER_DATA_PORT;
  } 
  else 
  {
    port = PIC_SLAVE_DATA_PORT;
    intr -= 8;
  }
  value = inb(port) & ~(1 << intr);
  outb(port, value);        
}

void i8259_remap_vectors(uint8_t master_offset, uint8_t slave_offset) {
	uint8_t slave_mask_cache, master_mask_cache;
	
	master_mask_cache = inb(PIC_MASTER_DATA_PORT);
	slave_mask_cache = inb(PIC_SLAVE_DATA_PORT);
	
	outb(PIC_MASTER_COMMAND_PORT, ICW1_INIT | ICW1_ICW4);
	iowait();
	outb(PIC_SLAVE_COMMAND_PORT, ICW1_INIT | ICW1_ICW4);
	iowait();
	outb(PIC_MASTER_DATA_PORT, master_offset);
	iowait();
	outb(PIC_SLAVE_DATA_PORT, slave_offset);
	iowait();
	outb(PIC_MASTER_DATA_PORT, 4);
	iowait();
	outb(PIC_SLAVE_DATA_PORT, 2);
	iowait();
	
	outb(PIC_MASTER_DATA_PORT, ICW4_8086);
	iowait();
	outb(PIC_SLAVE_DATA_PORT, ICW4_8086);
	iowait();
	
	outb(PIC_MASTER_DATA_PORT, master_mask_cache);
	outb(PIC_SLAVE_DATA_PORT, slave_mask_cache);
  for(int i = 0; i < 42; i++)
    i8259_unmask(i);
}

void i8259_send_eoi(uint8_t trapnr)
{
	if(trapnr >= 0x28)
		outb(PIC_SLAVE_COMMAND_PORT, PIC_COMMAND_END_OF_INTR);
	outb(PIC_MASTER_COMMAND_PORT, PIC_COMMAND_END_OF_INTR);
}
