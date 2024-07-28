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

const char possible_inits[][128] = {
  // /init is already tried, no point in trying it again
  "/bin/init", "/etc/init", "/etc/initrc", "/sbin/init", "/boot/init", "/usr/bin/init", "/usr/sbin/init", "/usr/local/bin/init"
};

int try_init(const char * path)
{
  int rc;
  struct file * file = (struct file *)malloc(sizeof(struct file));
  rc = vfs_open(file, init_path);
  if(IS_ERR(rc))
    return rc;
  /*
  uint8_t * program_buffer = (uint8_t *)malloc(file->inode->length);
  vfs_read(file, program_buffer, file->inode->length);
  elf_exec((const char *)init_path, program_buffer);
  */
  return 0;
}

int start_init(const char * cmdline)
{
  set_tss_stack(get_current_task()->esp);
  int rc;
  printk("Getting ready for init process.. Everybody, put on your safety helmets.\n");
  char * init_val = get_cmdline_param_val((char *)cmdline, "init");
  if(!init_val)
    strncpy(init_path, "/init", NAME_MAX);
  else
    strncpy(init_path, init_val, NAME_MAX);
  for(;;);
  printk("%s: trying %s...\n", __FUNCTION__, init_path);
  if (try_init(init_path) < 0)
  {
    for(int i = 0; i < 8; i++)
    {
      printk("%s: trying %s...\n", __FUNCTION__, possible_inits[i]);
      try_init(possible_inits[i]);
    }
  }

  // R.I.P usleep function, you shall be missed 

  // No valid init found. Filesystems have already been waited on, no point in staying up.
  panic("No valid init process found. Tried: %s %s %s %s %s %s %s %s", possible_inits[0], possible_inits[1], possible_inits[2], possible_inits[3], possible_inits[4], possible_inits[5], possible_inits[6], possible_inits[7]);
  return 0;
}