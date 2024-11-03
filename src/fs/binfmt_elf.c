#include <catk/elf.h>
#include <catk/mem.h>
#include <catk/fs.h>
#include <catk/types.h>
#include <catk/task.h>
#include <catk/errno.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/virt.h>
#include <stdint.h>
#include <lib/common.h>

extern void jmp_to_elf(uint32_t addr);

static bool elf_verify(const uint8_t * data)
{
  return (data[0] == 0x7f && data[1] == 0x45 && data[2] == 0x4c && data[3] == 0x46);
}

int elf_exec(const char * name, const uint8_t * data)
{
  uint32_t load_loc = 0;
  if(!elf_verify(data))
    return -ENOEXEC;
  struct elf_hdr * header = (struct elf_hdr *)data;
  struct elf_phdr * prghdr = (struct elf_phdr *)(data + header->e_phoff);
  for(int i = 0; i < header->e_phnum; i++, prghdr++)
  {
    switch(prghdr->p_type)
    {
      case ELF_TYPE_LOAD:
      {
        debug("elf file load offest 0x%08x..\n", prghdr->p_offset);
        load_loc = (uint32_t)(data + prghdr->p_offset);
        break;
      }
      default:
      {
        break;
      }
    }
  }
  if(!load_loc)
    return -ENOEXEC;
  char * argv[] = {(char *)name};
  spawn_user_task((char *)name, (void *)(load_loc + header->e_entry), TASK_PRIORITY_NORMAL, 1, argv);
  return 0;
}
