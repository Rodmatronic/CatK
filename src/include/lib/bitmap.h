#ifndef __BITMAP_H
#define __BITMAP_H

#include <stdint.h>

void bitmap_set(uint32_t * bitmap, int n);
void bitmap_unset(uint32_t * bitmap, int n);
int bitmap_test(uint32_t * bitmap, int n);

#endif
