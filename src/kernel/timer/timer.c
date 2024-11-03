#include <catk/core.h>
#include <catk/task.h>
#include <catk/io.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <lib/common.h>

volatile static uint32_t jiffies;

static void timer_configure_hz(uint16_t hz)
{ 
  int divisor = PIT_FREQ_DIVISOR / hz;  
  outb(0x43, 0x36); 
  outb(0x40, divisor & 0xff);
  outb(0x40, divisor >> 8);
}

void timer_irq(struct intr_stack_frame * regs)
{
  jiffies++;
  if(is_tasking_enabled())
  {
    schedule(regs);
  }
}

void timer_init(void)
{
  timer_configure_hz(1000);
  interrupt_install(timer_irq, 32);
}

void msleep(uint32_t ms) {
    uint32_t end = jiffies + ms;
    while (jiffies < end);
}
