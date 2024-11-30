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
#include <catk/trace.h>
#include <lib/common.h>

static const char possible_inits[][128] = {
  "/init", "/bin/init", "/etc/init", "/etc/initrc", "/sbin/init", "/boot/init", "/usr/bin/init", "/usr/sbin/init", "/usr/local/bin/init"
};

static char ** generate_argv(const char * exec_path) {
  char **argv = (char **)calloc(ARG_MAX, sizeof(char *));
  assert(argv != NULL);
  argv[0] = strdup(exec_path);
  argv[1] = NULL;
  return argv;
};

static int try_init(const char * path)
{
  int rc;
  struct file * file = (struct file *)malloc(sizeof(struct file));
  assert(file != NULL);
  rc = vfs_open(file, path);
  if(IS_ERR(rc)) {
    debug("open failed: %d\n", rc);
    return rc;
  }
  uint8_t * program_buffer = (uint8_t *)malloc(file->inode->length);
  rc = vfs_read(file, program_buffer, file->inode->length);
  if(IS_ERR(rc)) {
    debug("read failed: %d\n", rc);
    return rc;
  }
  debug("found file: %s\n", path);
  char **argv = generate_argv(path);

  /* TODO: pass argc to the function below */
  rc = load_elf_binary(file, argv);
  if(IS_ERR(rc)) {
    printk("exec failed: %d\n", rc);
  }
  vfs_close(file);
  return rc;
}

int start_init(const char * cmdline)
{
  /*
  printk("Reading file '/CONTENT'\n\n");
  struct file * file = (struct file *)malloc(sizeof(struct file));
  assert(file != NULL);
  int rc = vfs_open(file, "/CONTENT");
  if(IS_ERR(rc)) {
    return rc;
  }
  uint8_t * file_data = (uint8_t *)malloc(file->inode->length);
  rc = vfs_read(file, file_data, file->inode->length);
  if(IS_ERR(rc)) {
    return rc;
  }
  printk((const char *)file_data);
  return 0;
  */
  int rc;
  printk("Getting ready for init process.. Everybody, put on your safety helmets.\n");
  char * init_path = get_cmdline_param_val((char *)cmdline, "init");
  if(init_path != NULL) {
    rc = try_init(init_path);
    if(!IS_ERR(rc)) {
      return 0;
    }
  }
  for(int i = 0; i < 9; i++) {
    rc = try_init(possible_inits[i]);
    if(IS_ERR(rc) == false) {
      return 0;
    }
  }
  panic("No init executable found. Please repack your initramfs with a valid init executable.\n");
  unreachable;
}
