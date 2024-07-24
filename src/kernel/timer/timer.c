#include <catk/core.h>
#include <catk/task.h>
#include <catk/io.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <lib/common.h>

volatile uint32_t jiffies;

static void timer_configure_hz(uint16_t hz)
{ 
  int divisor = 1193180 / hz;  
  outb(0x43, 0x36); 
  outb(0x40, divisor & 0xff);
  outb(0x40, divisor >> 8);
}

void timer_irq(struct intr_stack_frame * regs)
{
  jiffies++;
  if(tasking_enabled())
  {
    struct task * p = get_current_task();
    /* the task still has a slice of cpu time left */
    if(p->ticks_left)
    {
      p->ticks_left--;
    }
    else
    {
      /* reset the time slice and switch to a new task */
      p->ticks_left = p->time_quantum;
      pic_eoi(0x20);
      schedule();
    }
  }
}

void timer_init(void)
{
  timer_configure_hz(1000);
  interrupt_install(timer_irq, 32);
}
