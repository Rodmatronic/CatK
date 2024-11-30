#ifndef __BITMAP_H
#define __BITMAP_H

#include <catk/types.h>

struct bitmap {
  uintptr_t * addr;
  size_t size;
};

void bitmap_set(struct bitmap * bitmap, int n);
void bitmap_unset(struct bitmap * bitmap, int n);
int bitmap_test(struct bitmap * bitmap, int n);

#endif
