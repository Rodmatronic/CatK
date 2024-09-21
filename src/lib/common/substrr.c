#include <lib/common.h>

void substrr(int s, int e, char * src, char * dest)
{
	memcpy(dest, &src[s], e - s);
	dest[e - s] = '\0';
}
