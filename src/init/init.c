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
  uint8_t * program_buffer = (uint8_t *)malloc(file->inode->length);
  rc = vfs_read(file, program_buffer, file->inode->length);
  if(IS_ERR(rc)) {
    return rc;
  }
  return elf_exec((const char *)init_path, program_buffer);
}

int start_init(const char * cmdline)
{
  int rc;
  set_tss_stack(get_current_task()->esp);
  printk("Getting ready for init process.. Everybody, put on your safety helmets.\n");
  char * init_val = get_cmdline_param_val((char *)cmdline, "init");
  if(!init_val)
    strncpy(init_path, "/init", NAME_MAX);
  else
    strncpy(init_path, init_val, NAME_MAX);
  printk("%s: trying %s...\n", __FUNCTION__, init_path);
  /* before i continue fixing things, i want to test the kernel heap poisoning */
  if (try_init(init_path) < 0)
  {
    for(int i = 0; i < 8; i++)
    {
      printk("%s: trying %s...\n", __FUNCTION__, possible_inits[i]);
      if (try_init(possible_inits[i]) == 0)
        return 0;
      printk("Hiss.. that didn't work..\n");
    }
    // R.I.P usleep function, you shall be missed 
    // No valid init found. Filesystems have already been waited on, no point in staying up.
    panic("No possible init programs");
  }
  else
  {
    return 0;
  }
  unreachable;
}
