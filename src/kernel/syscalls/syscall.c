#include <catk/core.h>
#include <catk/compiler.h>
#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/tty.h>
#include <catk/task.h>
#include <lib/common.h>
#include <config.h>
#include <stdint.h>

#if CATK_SYSCALL_TRACE == 1

void syscall_trace(struct intr_stack_frame * regs)
{
  debug("Syscall trace:\n");
  debug("\tEAX: 0x%08x, EBX: 0x%08x, ECX: 0x%08x, EDX: 0x%08x\n",
      regs->eax, regs->ebx, regs->ecx, regs->edx);
}

#endif

static uint32_t do_system_call(struct intr_stack_frame * regs)
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
      rc = sys_write((int)regs->ebx, (char *)regs->ecx, (size_t)regs->edx);
      break;
    }
    case 0x04:
    {
      rc = sys_open((const char *)regs->ebx, (int)regs->ecx, (uint16_t)regs->edx);
      break;
    }
    case 0x05: {
      rc = sys_fork();
      break; 
    }
    case 0x06: {
      rc = sys_access((const char *)regs->ebx, (int)regs->ecx);
      break; 
    }
    case 0x07: {
      rc = sys_wait((int *)regs->ebx);
      break; 
    }
    case 0x08: {
      rc = sys_chdir((const char *)regs->ebx);
      break; 
    }
    case 0x09: {
      rc = sys_execve((const char *)regs->ebx, (char ** const)regs->ecx, (char ** const)regs->edx);
      break; 
    }
    case 0x0a: {
      sys_beep((int)regs->ebx);
      rc = 0;
      break;
    }
    case 0x0b: {
      rc = sys_getpid();
      break;
    }
    case 0x0c: {
      rc = get_current_task()->argc;
      break;
    }
    case 0x0d: {
      struct task * p = get_current_task();
      for(int i = 0; i < p->argc; i++) {
        ((char **)regs->ebx)[i] = p->argv[i];
      }
      rc = 0;
      break;
    }
    default:
    {
      debug("Received bad or unsupported system call 0x%x\n", regs->eax);
      break;
    }
  }
  return rc;
}

void system_call(struct intr_stack_frame * regs)
{
#if CATK_SYSCALL_TRACE == 1
  syscall_trace(regs);
#endif
  regs->eax = do_system_call(regs);
}

void syscall_install(void)
{
  interrupt_install(system_call, 0x80);
}
