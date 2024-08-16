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

static void debug_print_info(struct elf_hdr * header)
{
	debug("elf header info:\n");
	debug("\tformat: %s\n", header->e_ident[4] ? "32-bit" : "64-bit");
	debug("\tendianness: %s\n", header->e_ident[5] ? "little endian" : "big endian");
	debug("\telf version: %d\n", header->e_ident[6]);
	debug("\tos abi: 0x%x\n", header->e_ident[7]);
	debug("\tobject file type: 0x%x\n", header->e_type);
	debug("\tmachine: 0x%x\n", header->e_machine);
	debug("\tentry point: 0x%x\n", header->e_entry);
}

int elf_exec(const char * name, const uint8_t * data)
{
  uint32_t load_loc = 0;
  if(!elf_verify(data))
    return -ENOEXEC;
  debug("elf load start\n");
  debug("elf data start: 0x%08x\n", data);
  struct elf_hdr * header = (struct elf_hdr *)data;
  debug_print_info(header);
  struct elf_phdr * prghdr = (struct elf_phdr *)(data + header->e_phoff);
  struct task * p = NULL;
  for(int i = 0; i < header->e_phnum; i++, prghdr++)
  {
    switch(prghdr->p_type)
    {
      case ELF_TYPE_LOAD:
      {
        debug("elf file load offest 0x%08x..\n", prghdr->p_offset);
        load_loc = (uint32_t)(data + prghdr->p_offset);
        uint32_t p_addr = load_loc, size = ALIGN(PAGE_ALIGNMENT, prghdr->p_filesz);
        p = create_user_task((char *)name, (load_loc + header->e_entry), TASK_PRIORITY_NORMAL);
        if(!p)
          return -ENOMEM;
        for(int i = 0; size > 0; p_addr += PAGE_ALIGNMENT, size -= PAGE_ALIGNMENT, i++) {
          uvm_map((uint32_t)p->cr3, i * PAGE_SIZE, p_addr);
        }
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
  task_add_queue(p);
  //asm volatile ("jmp *%0" :: "r"(load_loc + header->e_entry) : "eax");
  return 0;
}
