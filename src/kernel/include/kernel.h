#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define	KERNEL_BOOT_TAG	"---<<CATKERNEL BOOT>>---"

typedef struct {
    struct {
        uint32_t k_start_addr;
        uint32_t k_end_addr;
        uint32_t k_len;
        uint32_t text_start_addr;
        uint32_t text_end_addr;
        uint32_t text_len;
        uint32_t data_start_addr;
        uint32_t data_end_addr;
        uint32_t data_len;
        uint32_t rodata_start_addr;
        uint32_t rodata_end_addr;
        uint32_t rodata_len;
        uint32_t bss_start_addr;
        uint32_t bss_end_addr;
        uint32_t bss_len;
    } kernel;

    struct {
        uint32_t total_memory;
    } system;

    struct {
        uint32_t start_addr;
        uint32_t end_addr;
        uint32_t size;
    } available;
} KERNEL_MEMORY_MAP;

extern KERNEL_MEMORY_MAP g_kmap;