#include <catk/elf.h>
#include <catk/mem.h>
#include <catk/fs.h>
#include <catk/types.h>
#include <catk/task.h>
#include <catk/errno.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <stdint.h>
#include <lib/common.h>

static bool elf_verify(uint8_t * data)
{
  return (data[0] == 0x7f && data[1] == 0x45 && data[2] == 0x4c && data[3] == 0x46);
}

static void elf_debug_print_info(struct elf_hdr * header)
{
	debug("[elf] file info:\n");
	debug(" [+] format: %s\n", header->e_ident[4] ? "32-bit" : "64-bit");
	debug(" [+] endianness: %s\n", header->e_ident[5] ? "little endian" : "big endian");
	debug(" [+] elf version: %d\n", header->e_ident[6]);
	debug(" [+] os abi: 0x%x\n", header->e_ident[7]);
	debug(" [+] object file type: 0x%x\n", header->e_type);
	debug(" [+] machine: 0x%x\n", header->e_machine);
	debug(" [+] entry point: 0x%x\n", header->e_entry);
}

int elf_exec(const char * name, uint8_t * data)
{
  uint32_t load_loc, text_section_sz;
  if(!elf_verify(data))
    return -ENOEXEC;
  debug("[elf] load start\n");
  debug("[elf] elf data start: 0x%08x\n", data);
  struct elf_hdr * header = (struct elf_hdr *)data;
  elf_debug_print_info(header);
  struct elf_phdr * prghdr = (struct elf_phdr *)(data + header->e_phoff);
  debug("[elf] section .text size: %d bytes\n", prghdr->p_filesz);
  text_section_sz = prghdr->p_filesz;
  for(int i = 0; i < header->e_phnum; i++, prghdr++)
  {
    switch(prghdr->p_type)
    {
      case ELF_TYPE_LOAD:
      {
        debug("[elf] load offest 0x%08x..\n", prghdr->p_offset);
        load_loc = (uint32_t)data + prghdr->p_offset;
        break;
      }
      default:
      {
        break;
      }
    }
  }
  debug("[elf] load location 0x%08x\n", load_loc + header->e_entry);
  debug("[elf] jumping to entry point..\n");
  void * load = (void *)(load_loc + header->e_entry);
  spawn_user_task((char *)name, load, TASK_PRIORITY_NORMAL);
  for(;;);
  return 0;
}
