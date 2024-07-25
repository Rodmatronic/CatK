#include <catk/printk.h>
#include <catk/kernel.h>
#include <catk/debug.h>
#include <catk/vfs.h>
#include <catk/mem.h>
#include <catk/limits.h>
#include <catk/errno.h>
#include <catk/task.h>
#include <catk/ramdisk.h>
#include <catk/params.h>
#include <catk/elf.h>
#include <catk/core.h>
#include <catk/trace.h>
#include <lib/common.h>

char init_path[NAME_MAX + 1]; /* either set by cmdline or set by the kernel */

int start_init(const char * cmdline)
{
  set_tss_stack(get_current_task()->esp);
  int rc;
  debug("[kernel] %s start. be ready for every (possible) last minute bug.\n", __FUNCTION__);
  printk("Getting ready for init process.. Everybody, put on your safety helmets.\n");
  char * init_val = get_cmdline_param_val((char *)cmdline, "init");
  if(!init_val)
    strncpy(init_path, "/init", NAME_MAX);
  else
    strncpy(init_path, init_val, NAME_MAX);
  printk("%s: trying %s...\n", __FUNCTION__, init_path);
  printk("ELF files do not work (yet).\n");
  return 0;
  /*
  struct file * file = (struct file *)malloc(sizeof(struct file));
  rc = vfs_open(file, init_path);
  if(IS_ERR(rc))
    return rc;
  uint8_t * program_buffer = (uint8_t *)malloc(file->inode->length);
  vfs_read(file, program_buffer, file->inode->length);
  elf_exec((const char *)init_path, program_buffer);
  return 0;
  */
}
