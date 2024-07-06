#ifndef __VIRT_H
#define __VIRT_H

#include <catk/compiler.h>
#include <lib/common.h>

struct page_directory
{
  uint32_t present : 1;
  uint32_t read_write : 1;
  uint32_t user_mode : 1;
  uint32_t write_through : 1;
  uint32_t cache : 1;
  uint32_t accessed : 1;
  uint32_t dirty : 1;
  uint32_t page_size : 1;
  uint32_t global : 1;
  uint32_t available : 3;
  uint32_t frame : 20;
}_packed_;

struct page_table
{
  uint32_t present : 1;
  uint32_t read_write : 1;
  uint32_t user_mode : 1;
  uint32_t write_through : 1;
  uint32_t cache : 1;
  uint32_t accessed : 1;
  uint32_t dirty : 1;
  uint32_t page_size : 1;
  uint32_t global : 1;
  uint32_t available : 3;
  uint32_t frame : 20;
}_packed_;

void paging_init(void);

#endif
