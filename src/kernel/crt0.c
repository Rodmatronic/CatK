#include <multiboot2.h>
#include <catk/params.h>
#include <catk/debug.h>
#include <catk/virt.h>
#include <lib/common.h>
#include <stdint.h>

extern int kmain(int argc, char * argv[]);

void kernel_crt0(uintptr_t mbi) {
  int rc = 0;
  debug("Bootloader information pointer: 0x%08x\n", mbi);
  if(!multiboot2_validate_mbi(mbi)) {
    debug("Bootloader sent us with a bad multiboot2 information.\n");
    return; /* return into the infinite halt state */
  }
  multiboot2_set_mbi(mbi);
  /* turn cmdline into a bunch of arguments */
  char * args[512];
  int arg_count = 0;
  char * token = strtok(obtain_cmdline(), " ");
  while (token != NULL) {
    if(arg_count >= 511)
      break;
    args[arg_count++] = token;
    token = strtok(NULL, " ");
  }
  args[arg_count] = NULL;
  rc = kmain(arg_count, args);
  debug("kmain() returned an error code: %d", rc);
}
