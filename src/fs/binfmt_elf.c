#include <catk/elf.h>
#include <catk/mem.h>
#include <catk/fs.h>
#include <catk/types.h>
#include <catk/task.h>
#include <catk/errno.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/virt.h>
#include <catk/compiler.h>
#include <stdint.h>
#include <lib/common.h>

static inline bool is_elf_valid(struct elf_hdr * header) {
  return (header->e_ident[0] == 0x7f && header->e_ident[1] == 'E' && header->e_ident[2] == 'L' && header->e_ident[3] == 'F');
}

static void placeholder_entry(void) {
  debug("Fizzbuzz");
  die();
}

int load_elf_binary(struct file * file, char * argv[])
{
  if(file == NULL) {
    return -EFAULT;
  }
  struct elf_hdr * header_buf = (struct elf_hdr *)malloc(sizeof(struct elf_hdr)); 
  if(header_buf == NULL) {
    return -ENOMEM;
  }
  file->ops->lseek(file, 0, SEEK_SET);
  file->ops->read(file, (void *)header_buf, sizeof(struct elf_hdr));
  debug("Verifying ELF header..\n");
  if(is_elf_valid(header_buf) == false) {
    debug("This file isn't an executable\n");
    free(header_buf);
    return -ENOEXEC;
  }
  debug("ELF header is valid! :)\n");
  /* Do some extra checks just to be safe.. */
  if(header_buf->e_ident[4] == 2) {
    debug("64-bit code on a 32-bit platform is not supported!\n");
    free(header_buf);
    return -ENOEXEC;
  }
  
  if(header_buf->e_type != 2) {
    debug("ELF binary is NOT executable!\n");
    free(header_buf);
    return -ENOEXEC;
  }

  if(header_buf->e_machine != 0x03) {
    debug("Unknown or unsupported ISA!\n");
    free(header_buf);
    return -ENOEXEC;
  }
  
  debug("Searching for LOAD segment...\n");

  struct elf_phdr * program_header = (struct elf_phdr *)calloc(header_buf->e_phnum, sizeof(struct elf_phdr));
  if(program_header == NULL) {
    free(header_buf);
    return -ENOMEM;
  }
  
  file->ops->lseek(file, header_buf->e_phoff, SEEK_SET);
  file->ops->read(file, (void *)program_header, sizeof(struct elf_phdr) * header_buf->e_phnum);

  uint32_t elf_code_offset = 0;
  uint32_t elf_code_virt_addr = 0;
  size_t elf_code_size = 0;

  for(size_t i = 0; i < header_buf->e_phnum; i++, program_header++) {
    switch(program_header->p_type) {
      case ELF_TYPE_LOAD: {
        debug("Found LOAD segment.\n");
        debug("LOAD segment offset: 0x%08x\n", program_header->p_offset);
        elf_code_offset = program_header->p_offset;
        elf_code_size = program_header->p_filesz;
        elf_code_virt_addr = program_header->p_vaddr;
        break;
      }
      default: {
        break;
      }
    }
  }
  
  debug("ELF code offset: 0x%08x\n", elf_code_offset);

  /* now we get ready for loading */
  uint8_t * text_section = (uint8_t *)malloc(elf_code_size);
  file->ops->lseek(file, elf_code_offset, SEEK_SET);
  file->ops->read(file, (void *)text_section, elf_code_size);
  struct task * p = create_user_task(file->name, placeholder_entry, TASK_PRIORITY_NORMAL, 1, argv);
  if(p == NULL) {
    return -ENOMEM;
  }
  if(p->pgd == 0) {
    return -ENOMEM;
  }
  vmm_map_user_region(&p->pgd, (uint32_t)&text_section, elf_code_virt_addr, elf_code_size, 0);
  free(header_buf);
  free(program_header);
  return 0;
}
