#include <stdint.h>
#include <catk/bitops.h>
#include <lib/common.h>
#include <lib/bitmap.h>

#define BIT_OFFSET(bit) (bit % BITS_PER_TYPE(uint32_t))

void bitmap_set(uint32_t * bitmap, int n) {
  bitmap[BITS_TO_U32(n)] |= BIT(BIT_OFFSET(n));
}

void bitmap_unset(uint32_t * bitmap, int n) {
  bitmap[BITS_TO_U32(n)] &= ~BIT(BIT_OFFSET(n));
}

int bitmap_test(uint32_t * bitmap, int n) {
  return bitmap[BITS_TO_U32(n)] & BIT(BIT_OFFSET(n));
}
