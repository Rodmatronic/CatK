#include <catk/virt.h>
#include <lib/common.h>
#include <stdint.h>

#define PAGE_SIZE 4096
#define PAGE_ALIGN(num) (num >> 12)

static struct page_table page_tables[1024] _aligned(PAGE_SIZE);
static struct page_directory page_dir[1024] _aligned(PAGE_SIZE);

extern void paging_enable(void);

static void setup_page_tables(void)
{
  int i;
  memset(page_tables, 0, sizeof(struct page_table) * 1024);
  memset(page_dir, 0, sizeof(struct page_directory) * 1024);
  for(i = 0; i < 1024; i++)
  {
    page_dir[i].read_write = 1;
    page_dir[i].user_mode = 1;
  }
  for(i = 0; i < 1024; i++)
  {
    page_tables[i].present = 1;
    page_tables[i].read_write = 1;
    page_tables[i].user_mode = 1;
    page_tables[i].frame = PAGE_ALIGN((i * 0x40000000));
  }
  page_dir[0].present = 1;
  page_dir[0].accessed = 0;
  page_dir[0].user_mode = 1;
  page_dir[0].frame = PAGE_ALIGN((uint32_t)page_tables);
}

static void load_page_directory(struct page_directory * pd)
{
  uint32_t cr0;
  asm volatile("mov %0, %%cr3" ::"r"((uint32_t)pd));
  // set bit in cr0 to enable paging
  asm volatile("mov %%cr0, %0": "=r"(cr0));
  cr0 = cr0 | 0x80000001;
  asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

void paging_init(void)
{
  /*
  setup_page_tables();
  load_page_directory((struct page_directory *)page_dir);
  */
}
