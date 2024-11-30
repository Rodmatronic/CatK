#include <catk/debug.h>
#include <catk/bitops.h>
#include <lib/common.h>
#include <lib/bitmap.h>
#include <stdint.h>

#define BIT_OFFSET(bit) (bit % BITS_PER_TYPE(uint32_t))

void bitmap_set(struct bitmap * bitmap, int n) {
  assert(BITS_TO_U32(n) <= bitmap->size);
  bitmap->addr[BITS_TO_U32(n)] |= BIT(BIT_OFFSET(n));
}

void bitmap_unset(struct bitmap * bitmap, int n) {
  assert(BITS_TO_U32(n) <= bitmap->size);
  bitmap->addr[BITS_TO_U32(n)] &= ~BIT(BIT_OFFSET(n));
}

int bitmap_test(struct bitmap * bitmap, int n) {
  assert(BITS_TO_U32(n) <= bitmap->size);
  return bitmap->addr[BITS_TO_U32(n)] & BIT(BIT_OFFSET(n));
}
