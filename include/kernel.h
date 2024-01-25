#ifndef KERNEL_H
#define KERNEL_H
#define PORT 0x3f8          // COM1
#include "types.h"
#include "timer.h"

typedef struct {
    void (*func)();
    void *stack;
} Process;

void kernmessage(const char* str);
void wait_seconds(int seconds);
void boot();
//static int init_serial();
int is_transmit_empty();
void write_serial(const char* str);
void pserial(const char* str);
void daemon(TIMER_FUNCTION function, uint32 timeout);

Process *fork(void (*func)(void));

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


typedef struct {
    struct {
        uint32 k_start_addr;
        uint32 k_end_addr;
        uint32 k_len;
        uint32 text_start_addr;
        uint32 text_end_addr;
        uint32 text_len;
        uint32 data_start_addr;
        uint32 data_end_addr;
        uint32 data_len;
        uint32 rodata_start_addr;
        uint32 rodata_end_addr;
        uint32 rodata_len;
        uint32 bss_start_addr;
        uint32 bss_end_addr;
        uint32 bss_len;
    } kernel;

    struct {
        uint32 total_memory;
    } system;

    struct {
        uint32 start_addr;
        uint32 end_addr;
        uint32 size;
    } available;
} KERNEL_MEMORY_MAP;

extern KERNEL_MEMORY_MAP g_kmap;

#endif