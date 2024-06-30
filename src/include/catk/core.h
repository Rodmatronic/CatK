#ifndef __CORE_H
#define __CORE_H

#include <catk/compiler.h>
#include <stdint.h>

/* GDT */

#define GDT_NUM_DESCRIPTORS 8

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

/*IDT */

struct idt_reg
{
  uint16_t size;
  uint32_t offset;
}_packed_;

struct idt_vector
{
  uint16_t base_lo;
  uint16_t segm_selector;
  uint8_t reserved;
  uint8_t flags;
  uint16_t base_hi;
}_packed_;

#define IDT_NUM_ENTRIES 256

struct intr_stack_frame
{
  uint32_t ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; 
  uint32_t intr, err_code;
  uint32_t eip, cs, eflags, useresp, ss;
};

typedef void (*intr_handler)(struct intr_stack_frame *);
void interrupt_install(intr_handler handler, int intr);

extern void interrupt_0();
extern void interrupt_1();
extern void interrupt_2();
extern void interrupt_3();
extern void interrupt_4();
extern void interrupt_5();
extern void interrupt_6();
extern void interrupt_7();
extern void interrupt_8();
extern void interrupt_9();
extern void interrupt_10();
extern void interrupt_11();
extern void interrupt_12();
extern void interrupt_13();
extern void interrupt_14();
extern void interrupt_15();
extern void interrupt_16();
extern void interrupt_17();
extern void interrupt_18();
extern void interrupt_19();
extern void interrupt_20();
extern void interrupt_21();
extern void interrupt_22();
extern void interrupt_23();
extern void interrupt_24();
extern void interrupt_25();
extern void interrupt_26();
extern void interrupt_27();
extern void interrupt_28();
extern void interrupt_29();
extern void interrupt_30();
extern void interrupt_31();
extern void interrupt_32();
extern void interrupt_33();
extern void interrupt_34();
extern void interrupt_35();
extern void interrupt_36();
extern void interrupt_37();
extern void interrupt_38();
extern void interrupt_39();
extern void interrupt_40();
extern void interrupt_41();
extern void interrupt_42();
extern void interrupt_43();
extern void interrupt_44();
extern void interrupt_45();
extern void interrupt_46();
extern void interrupt_47();
extern void syscall_dispatcher();

/* PIC */

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20
#define ICW1 0x11
#define ICW4_8086 0x01

void pic_eoi(uint8_t irq);

/* PIT */

void timer_init(void);

/* Common functions */

void critical_enter(void);
void critical_exit(void);

void cpu_init(void);

#endif
