#ifndef __ELF_H
#define __ELF_H

#include <stdint.h>

#define ELF_TYPE_LOAD 1

struct elf_hdr
{
	uint8_t  e_ident[16];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint32_t e_entry;
	uint32_t e_phoff;
	uint32_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
};

struct elf_phdr
{
	uint32_t p_type;
	uint32_t p_offset;
	uint32_t p_vaddr;
	uint32_t p_paddr;
	uint32_t p_filesz;
	uint32_t p_memsz;
	uint32_t p_flags;
	uint32_t p_align;
};

struct elf_section
{
	uint16_t	sh_name;
	uint16_t	sh_type;
	uint16_t	sh_flags;
	uint32_t	sh_addr;
	uint32_t	sh_offset;
	uint16_t	sh_size;
	uint16_t	sh_link;
	uint16_t	sh_info;
	uint16_t	sh_addralign;
	uint16_t	sh_entsize;
};

struct elf_symbol
{
	uint16_t		st_name;
	uint32_t		st_value;
	uint16_t		st_size;
	uint8_t			st_info;
	uint8_t			st_other;
	uint16_t		st_shndx;
};

int elf_do_load(const char * name, const uint8_t * data);

#endif
