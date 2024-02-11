#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "term.h"
#include "printk.h"
#include "panic.h"
#include "cpu.h"
#include <cpuid.h>
#include <config.h>
#include <string.h>

uint32_t eax, ebx, ecx, edx;

// takes in a 32-bit uint, a buffer, and an offset, and outputs the raw bytes
// of the number into the buffer. if the buffer has reached it's end, a null terminator will be placed.
void itos(uint32_t number, char* buffer, uint32_t offset) {
    for (unsigned int i = 0; i < sizeof(number); i++) {
        buffer[offset + i] = *((char *)&number + i);
    }
    
    if (offset + sizeof(number) == sizeof(buffer) - 1) {
        buffer[offset + sizeof(number)] = '\0';
    }
}

void cpuid_info() {
    char vendor[13];
    char model[49];
    int cores;

    // vendor
    __cpuid(0x00000000, eax, ebx, ecx, edx);
    itos(ebx, vendor, 0);
    itos(edx, vendor, sizeof(uint32_t) * 1);
    itos(ecx, vendor, sizeof(uint32_t) * 2);

    // model
    __cpuid(0x80000002, eax, ebx, ecx, edx);
    itos(eax, model, 0);
    itos(ebx, model, sizeof(uint32_t) * 1);
    itos(ecx, model, sizeof(uint32_t) * 2);
    itos(edx, model, sizeof(uint32_t) * 3);

    __cpuid(0x80000003, eax, ebx, ecx, edx);
    itos(eax, model, sizeof(uint32_t) * 4);
    itos(ebx, model, sizeof(uint32_t) * 5);
    itos(ecx, model, sizeof(uint32_t) * 6);
    itos(edx, model, sizeof(uint32_t) * 7);

    __cpuid(0x80000004, eax, ebx, ecx, edx);
    itos(eax, model, sizeof(uint32_t) * 8);
    itos(ebx, model, sizeof(uint32_t) * 9);
    itos(ecx, model, sizeof(uint32_t) * 10);
    itos(edx, model, sizeof(uint32_t) * 11);

    // number of cores
    __cpuid(0x00000001, eax, ebx, ecx, edx);
    cores = (ebx >> 16) & 0xFF;

    if (cores == 0) {
        // If the number of cores is reported as 0, we need to check if hyper-threading is enabled
        cores = (ebx >> 28) & 0xF;
        if (cores > 0) {
            // Hyper-threading is enabled, so the actual number of cores is half the number of logical processors
            cores *= 2;
        } else {
            // Hyper-threading is not enabled, so the number of cores remains 1
            cores = 1;
        }
    }

    printk("cpu: %s\n  %s\n  logical processors: %d\n", vendor, model, cores);
    strcpy(cpubrand, vendor);
    strcpy(cpumodel, model);
    cpulogicores = cores;
}