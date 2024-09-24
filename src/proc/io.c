#include <catk/printk.h>
#include <catk/kernel.h>
#include <catk/debug.h>
#include <catk/vfs.h>
#include <catk/mem.h>
#include <catk/limits.h>
#include <catk/errno.h>
#include <catk/task.h>
#include <catk/params.h>
#include <catk/elf.h>
#include <catk/core.h>
#include <lib/common.h>


int stdin_read(struct file * fptr, void * buf, size_t sz) {
  return 0;
}

int stdin_write(struct file * fptr, void * buf, size_t sz) {
  return 0;
}

int stdin_open(struct file * fptr, const char * path) {
  return 0;
}

void stdin_close(struct file * fptr) {
  return;
}

struct file_operations stdin_fops = {
  NULL,
  NULL,
  stdin_read,
  stdin_write,
  NULL,
  NULL,
  stdin_open,
  stdin_close
};
