#include <lib/common.h>

int memcmp(const void * addr1, const void * addr2, size_t count) {
  int ret = __builtin_memcmp(addr1, addr2, count);
  return ret;
}
