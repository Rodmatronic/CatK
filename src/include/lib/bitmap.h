#ifndef __BITMAP_H
#define __BITMAP_H

#include <stdint.h>

void bitmap_set(uint8_t * bitmap, int n);
void bitmap_unset(uint8_t * bitmap, int n);
int bitmap_test(uint8_t * bitmap, int n);

#endif
