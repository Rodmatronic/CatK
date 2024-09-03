#ifndef __PLATFORM_X86_I8259
#define __PLATFORM_X86_I8259

#include <stdint.h>

void i8259_remap_vectors(uint8_t offset1, uint8_t offset2);
void i8259_send_eoi(uint8_t trapnr);

#endif