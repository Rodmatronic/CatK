#ifndef KERNEL_H
#define KERNEL_H
#define PORT 0x3f8          // COM1
#include "types.h"

void bootlogo_splash();
void kernmessage(const char* str);
void wait_seconds(int seconds);
void boot();
static int init_serial();
int is_transmit_empty();
void write_serial(const char* str);
void pserial(const char* str);
// symbols from linker.ld for section addresses
extern uint8 __kernel_section_start;
extern uint8 __kernel_section_end;
extern uint8 __kernel_text_section_start;
extern uint8 __kernel_text_section_end;
extern uint8 __kernel_data_section_start;
extern uint8 __kernel_data_section_end;
extern uint8 __kernel_rodata_section_start;
extern uint8 __kernel_rodata_section_end;
extern uint8 __kernel_bss_section_start;
extern uint8 __kernel_bss_section_end;

#endif

