#ifndef __DIRENT_H
#define __DIRENT_H

#include <catk/types.h>
#include <catk/limits.h>
#include <stdint.h>

/* directory entry */
struct dirent
{
	uint32_t d_ino;
	off_t d_off;
	uint16_t d_reclen;
	char d_name[NAME_MAX + 1];
};

#endif
