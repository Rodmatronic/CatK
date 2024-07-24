#ifndef __VIRT_H
#define __VIRT_H

#include <catk/compiler.h>
#include <lib/common.h>

/* For more info about good old paging, download the Intel Software Developer Manual at https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html */

/* Page Directory Entry (PDE) values */
#define PDE_PRESENT         BIT(0)
#define PDE_RW              BIT(1)
#define PDE_USERMODE        BIT(2)
#define PDE_WRITETHROUGH    BIT(3)
#define PDE_CACHING         BIT(4)
#define PDE_ACCESSED        BIT(5)
#define PDE_DIRTY           BIT(6)
#define PDE_PAGESIZE        BIT(7)
#define PDE_IGNORED         (~(BIT(9) | BIT(10) | BIT(11)))
/* Page Table Entry (PTE) values */
#define PTE_PRESENT         PDE_PRESENT
#define PTE_RW              PDE_RW
#define PTE_USERMODE        PDE_USERMODE
#define PTE_WRITETHROUGH    PDE_WRITETHROUGH
#define PTE_CACHING         PDE_CACHING
#define PTE_ACCESSED        PDE_ACCESSED
#define PTE_DIRTY           PDE_DIRTY
#define PTE_PAT             BIT(7)
#define PTE_GLOBAL          BIT(8)
#define PTE_IGNORED         PDE_IGNORED

#define PAGE_ADDR(addr)     (addr << 12)

void paging_init(void);

#endif
