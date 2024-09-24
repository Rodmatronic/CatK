#include <catk/core.h>
#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/task.h>
#include <catk/printk.h>
#include <catk/vfs.h>
#include <catk/errno.h>
#include <catk/mem.h>
#include <catk/elf.h>
#include <stdint.h>

int sys_execve(const char * pathname, char * const argv[], char * const envp[]) {
  int rc;
  struct file * file = (struct file *)malloc(sizeof(struct file));
  rc = vfs_open(file, pathname);
  if(IS_ERR(rc))
    return rc;
  uint8_t * program_buffer = (uint8_t *)malloc(file->inode->length);
  vfs_read(file, program_buffer, file->inode->length);
  return elf_exec((const char *)pathname, program_buffer);
}
