#include <lib/common.h>
#include <sys/types.h>

void strncpy(char * dest, const char * src, size_t n)
{
	memcpy(dest, src, n);
	dest[n] = '\0';
}
