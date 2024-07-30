#include <catk/core.h>
#include <catk/compiler.h>
#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/tty.h>
#include <lib/common.h>
#include <stdint.h>

void syscall_trace(struct intr_stack_frame * regs)
{
  printk("syscall_trace: eax: 0x%08x, ebx: 0x%08x, ecx: 0x%08x, edx: 0x%08x\n", 
      regs->eax, regs->ebx, regs->ecx, regs->edx);
}

static uint32_t _system_call(struct intr_stack_frame * regs)
{
  uint32_t rc = -ENOSYS;
  switch(regs->eax)
  {
    case 0x00:
    {
      rc = 0;
      break;
    }
    case 0x01:
    {
      /* sometimes a task can return from this.. */
      sys_exit((int)regs->ebx);
      break;
    }
    case 0x02:
    {
      rc = sys_read((int)regs->ebx, (char *)regs->ecx, (size_t)regs->edx);
      break;
    }
    case 0x03:
    {
      printk((char *)regs->ebx);
      rc = 0;
      break;
    }
    case 0x04:
    {
      rc = sys_open((const char *)regs->ebx, (int)regs->ecx, (uint16_t)regs->edx);
      break;
    }
    default:
    {
      debug("syscall: bad or unsupported system call 0x%x\n", regs->eax);
      break;
    }
  }
  return rc;
}

void system_call(struct intr_stack_frame * regs)
{
  //syscall_trace(regs);
  regs->eax = _system_call(regs);
}

void syscall_install(void)
{
  debug("syscall: syscalls enabled.\n");
  interrupt_install(system_call, 128);
}
