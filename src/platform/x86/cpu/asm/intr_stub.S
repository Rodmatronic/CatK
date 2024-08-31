.section .text
  .global intr_start
  .global intr_stub_ignore
  .extern intr_handler

#define NO_ERR \
  pushl $0

#define HAS_ERR \
  nop

#define DEFINE_STUB(NUMBER, ERR)                      \
.section .text;                                  \
  .global intr_stub_##NUMBER##;  \
intr_stub_##NUMBER##:                            \
	ERR;                                   \
	pushl $NUMBER;                       \
  jmp intr_stub_start;                         \

intr_stub_start:
  pushl %ds
  pushal

  movw $0x10, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %fs
  movw %ax, %gs

  pushl %esp

  call intr_handler

intr_stub_end:
  popl %esp

  popal
  popl %eax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %fs
  movw %ax, %gs

  addl $0x8, %esp

  iretl

intr_stub_ignore:
  iretl

DEFINE_STUB(0, NO_ERR)
DEFINE_STUB(1, NO_ERR)
DEFINE_STUB(2, NO_ERR)
DEFINE_STUB(3, NO_ERR)
DEFINE_STUB(4, NO_ERR)
DEFINE_STUB(5, NO_ERR)
DEFINE_STUB(6, NO_ERR)
DEFINE_STUB(7, NO_ERR)
DEFINE_STUB(8, HAS_ERR)
DEFINE_STUB(9, NO_ERR)
DEFINE_STUB(10, HAS_ERR)
DEFINE_STUB(11, HAS_ERR)
DEFINE_STUB(12, HAS_ERR)
DEFINE_STUB(13, HAS_ERR)
DEFINE_STUB(14, HAS_ERR)
DEFINE_STUB(15, NO_ERR)
DEFINE_STUB(16, NO_ERR)
DEFINE_STUB(17, HAS_ERR)
DEFINE_STUB(18, NO_ERR)
DEFINE_STUB(19, NO_ERR)
DEFINE_STUB(20, NO_ERR)
DEFINE_STUB(21, HAS_ERR)
DEFINE_STUB(22, NO_ERR)
DEFINE_STUB(23, NO_ERR)
DEFINE_STUB(24, NO_ERR)
DEFINE_STUB(25, NO_ERR)
DEFINE_STUB(26, NO_ERR)
DEFINE_STUB(27, NO_ERR)
DEFINE_STUB(28, NO_ERR)
DEFINE_STUB(29, HAS_ERR)
DEFINE_STUB(30, HAS_ERR)
DEFINE_STUB(31, NO_ERR)
DEFINE_STUB(32, NO_ERR)