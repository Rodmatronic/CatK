#include <catk/printk.h>
#include <catk/kernel.h>
#include <catk/debug.h>
#include <catk/vfs.h>
#include <catk/mem.h>
#include <catk/limits.h>
#include <catk/errno.h>
#include <catk/task.h>
#include <catk/ramdisk.h>

char init_path[NAME_MAX + 1]; /* either set by cmdline or set by the kernel */

int start_init(void)
{
  int rc;
  debug("[kernel] %s start. be ready for every (possible) last minute bug.\n", __FUNCTION__);
  printk("Getting ready for init process.. Everybody, put on your safety helmets.\n");
  struct file * file = (struct file *)malloc(sizeof(struct file));
  rc = vfs_open(file, "/bin/test");
  if(IS_ERR(rc))
    return rc;
  uint8_t * program_buffer = (uint8_t *)calloc(file->inode->length, 1);
  vfs_read(file, program_buffer, file->inode->length);
  /* execute raw binary in kernel-mode */
  spawn_kernel_task(file->name, (uint32_t)program_buffer, TASK_PRIORITY_NORMAL);
  return 0;
}
