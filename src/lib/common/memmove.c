#include <catk/types.h>
#include <stdint.h>
#include <lib/common.h>

void * memmove(void * dest, const void * src, size_t n)
{
	uint8_t * d = (uint8_t *)dest;
	const uint8_t * s = (const uint8_t *)src;
	if (d < s) {
		memcpy(dest, src, n);
  } else {
    /* go in reverse */
    asm("std");
    memcpy(dest, src, n);
	}
  asm("cld");
	return dest;
}

void * memmove16(void * dest, const void * src, size_t n)
{
	uint16_t * d = (uint16_t *)dest;
	const uint16_t * s = (const uint16_t *)src;
	if (d < s) {
		memcpy16(dest, src, n);
  } else {
    /* go in reverse */
    asm("std");
    memcpy16(dest, src, n);
	}
  asm("cld");
	return dest;
}

void * memmove32(void * dest, const void * src, size_t n)
{
	uint32_t * d = (uint32_t *)dest;
	const uint32_t * s = (const uint32_t *)src;
	if (d < s) {
		memcpy32(dest, src, n);
  } else {
    /* go in reverse */
    asm("std");
    memcpy32(dest, src, n);
	}
  asm("cld");
	return dest;
}
