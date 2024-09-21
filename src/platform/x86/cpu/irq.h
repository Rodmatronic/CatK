#ifndef __PLATFORM_X86_IRQ
#define __PLATFORM_X86_IRQ

/* interrupts */

#define IRQ_START_VECTOR 32
#define IRQ(num) (IRQ_START_VECTOR + num)

/* trap interrupts */

#define X86_TRAP_DE		    0	  /* Divide-by-zero */
#define X86_TRAP_DB		    1	  /* Debug */
#define X86_TRAP_NMI	    2	  /* Non-maskable Interrupt */
#define X86_TRAP_BP		    3	  /* Breakpoint */
#define X86_TRAP_OF		    4	  /* Overflow */
#define X86_TRAP_BR		    5	  /* Bound Range Exceeded */
#define X86_TRAP_UD		    6	  /* Invalid Opcode */
#define X86_TRAP_NM		    7	  /* Device Not Available */
#define X86_TRAP_DF		    8	  /* Double Fault */
#define X86_TRAP_TS		    10	/* Invalid TSS */
#define X86_TRAP_NP		    11	/* Segment Not Present */
#define X86_TRAP_SS		    12	/* Stack Segment Fault */
#define X86_TRAP_GP		    13	/* General Protection Fault */
#define X86_TRAP_PF		    14	/* Page Fault */
#define X86_TRAP_SPURIOUS	15	/* Spurious Interrupt */
#define X86_TRAP_MF		    16	/* x87 Floating-Point Exception */
#define X86_TRAP_AC		    17	/* Alignment Check */
#define X86_TRAP_MC		    18	/* Machine Check */
#define X86_TRAP_XF		    19	/* SIMD Floating-Point Exception */
#define X86_TRAP_VE		    20	/* Virtualization Exception */
#define X86_TRAP_CP		    21	/* Control Protection Exception */
#define X86_TRAP_VC		    29	/* VMM Communication Exception */

extern void intr_stub_0(void);
extern void intr_stub_1(void);
extern void intr_stub_2(void);
extern void intr_stub_3(void);
extern void intr_stub_4(void);
extern void intr_stub_5(void);
extern void intr_stub_6(void);
extern void intr_stub_7(void);
extern void intr_stub_8(void);
extern void intr_stub_9(void);
extern void intr_stub_10(void);
extern void intr_stub_11(void);
extern void intr_stub_12(void);
extern void intr_stub_13(void);
extern void intr_stub_14(void);
extern void intr_stub_15(void);
extern void intr_stub_16(void);
extern void intr_stub_17(void);
extern void intr_stub_18(void);
extern void intr_stub_19(void);
extern void intr_stub_20(void);
extern void intr_stub_21(void);
extern void intr_stub_22(void);
extern void intr_stub_23(void);
extern void intr_stub_24(void);
extern void intr_stub_25(void);
extern void intr_stub_26(void);
extern void intr_stub_27(void);
extern void intr_stub_28(void);
extern void intr_stub_29(void);
extern void intr_stub_30(void);
extern void intr_stub_31(void);
extern void intr_stub_32(void);
extern void intr_stub_33(void);
extern void intr_stub_34(void);
extern void intr_stub_35(void);
extern void intr_stub_36(void);
extern void intr_stub_37(void);
extern void intr_stub_38(void);
extern void intr_stub_39(void);
extern void intr_stub_40(void);
extern void intr_stub_41(void);
extern void intr_stub_42(void);
extern void intr_stub_43(void);
extern void intr_stub_44(void);
extern void intr_stub_45(void);

bool is_idt_loaded(void);
void exceptions_install(void);

#endif
