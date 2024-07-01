#include <catk/core.h>
#include <catk/compiler.h>
#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <lib/common.h>
#include <stdint.h>

void syscall_trace(struct pt_regs * regs)
{
  printk("%s(): eax: 0x%08x, ebx: 0x%08x, ecx: 0x%08x, edx: 0x%08x\n", 
         __FUNCTION__, regs->orig_eax, regs->ebx, regs->ecx, regs->edx);
}

static uint32_t _system_call(struct pt_regs * regs)
{
  uint32_t rc = -ENOSYS;
  switch(regs->orig_eax)
  {
    case 0x00:
    {
      /* a task shouldn't return from this, so it wont make sense to give a return code */
      sys_exit((int)regs->ebx);
      break;
    }
    default:
    {
      debug("[syscall] bad or unsupported system call 0x%x\n", regs->orig_eax);
      break;
    }
  }
  return rc;
}

void system_call(struct pt_regs * regs)
{
  syscall_trace(regs);
  regs->eax = _system_call(regs);
}
