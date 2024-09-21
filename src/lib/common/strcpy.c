#include <lib/common.h>
#include <catk/types.h>

void strcpy(char * dest, const char * src)
{
	memcpy(dest, src, strlen(src));
	dest[strlen(src)] = '\0';
}
