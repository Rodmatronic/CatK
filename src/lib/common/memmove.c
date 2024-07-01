#include <catk/types.h>
#include <stdint.h>
#include <lib/common.h>

void * memmove(void * dest, const void * src, size_t n)
{
	uint8_t * d = (uint8_t *)dest;
	const uint8_t * s = (const uint8_t *)src;
	if (d < s) 
  {
		for (size_t i = 0; i < n; i++)
			d[i] = s[i];
	} 
  else 
  {
		for (size_t i = n; i != 0; i--)
			d[i-1] = s[i-1];
	}
	return dest;
}
