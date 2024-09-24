#ifndef __CORE_H
#define __CORE_H

#include <catk/compiler.h>
#include <stdint.h>

/* GDT */

struct segm_descriptor
{
  uint16_t limit;
  uint16_t base_low;
  uint8_t base_mid;
  uint8_t access;
  uint8_t flags;
  uint8_t base_high;      
}_packed_;

struct gdtr
{
  uint16_t size;
  uint32_t offset;
}_packed_;

/* IDT */

struct idtr
{
  uint16_t size;
  uint32_t offset;
}_packed_;

struct interrupt_vector
{
  uint16_t base_low;
  uint16_t segm_selector;
  uint8_t reserved;
  uint8_t flags;
  uint16_t base_high;
}_packed_;

struct intr_stack_frame {
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t tmp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  uint32_t ds;
  /* pushed by us */
  uint32_t trapnr;
  uint32_t error;
  /* given from the cpu */
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  uint32_t esp;
  uint32_t ss;
}_packed_;

int intr_add_handler(int vector, void (*intr)(struct intr_stack_frame *));

/* Functions that can be used on any machine */

int gen_random(void);
void critical_enter(void);
void critical_exit(void);
void halt(void);

void cpu_dump_all_info(void);
void early_platform_init(void);

#endif
