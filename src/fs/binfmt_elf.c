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

/*

static inline struct elf_section * elf_get_section_header(struct elf_hdr * header)
{
  return (struct elf_section *)((uint32_t)header + header->e_shoff);
}

static inline struct elf_section * elf_get_section(int idx, struct elf_hdr * header)
{
  return &elf_get_section_header(header)[idx];
}

static inline char * elf_get_string_table(struct elf_hdr * hdr)
{
	if(!hdr->e_shstrndx)
    return NULL;
	return (char *)hdr + elf_get_section(hdr->e_shstrndx, hdr)->sh_offset;
}

static inline char * elf_string_lookup(struct elf_hdr * hdr, int offset)
{
	char * strtab = elf_get_string_table(hdr);
	if(!strtab)
    return NULL;
	return strtab + offset;
}
static uint32_t elf_get_symval(struct elf_hdr * hdr, int table, int idx) 
{
	if(!table || !idx) 
    return -1;
	struct elf_section * symtab = elf_get_section(table, hdr);
	uint32_t symtab_entries = symtab->sh_size / symtab->sh_entsize;
	if(idx >= symtab_entries)
  {
    debug("elf: index is out of range. given index: %d, max index: %d\n", idx, symtab_entries);
		return -1;
	}
	int symaddr = (int)hdr + symtab->sh_offset;
	struct elf_symbol * symbol = &((struct elf_symbol *)symaddr)[idx];
  if(!symbol->st_shndx) 
  {
    debug("elf: external symbols are not supported");
    return -1;
  }
  else
  {
    debug("elf: symbol is defined :D");
  }
  return 0;
}

*/

int elf_exec(const char * name, const uint8_t * data)
{
  uint32_t load_loc, text_section_sz = 0;
  if(!elf_verify(data))
    return -ENOEXEC;
  debug("elf load start\n");
  debug("elf data start: 0x%08x\n", data);
  struct elf_hdr * header = (struct elf_hdr *)data;
  debug_print_info(header);
  struct elf_phdr * prghdr = (struct elf_phdr *)(data + header->e_phoff);
  debug("section .text size: %d bytes\n", prghdr->p_filesz);
  text_section_sz = prghdr->p_filesz;
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
  //spawn_user_task((char *)name, (load_loc + header->e_entry), TASK_PRIORITY_NORMAL);
  asm volatile ("jmp *%0" :: "r"(load_loc + header->e_entry) : "eax");
  return 0;
}
