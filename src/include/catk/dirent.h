#ifndef __DIRENT_H
#define __DIRENT_H

/* directory entry */
struct dirent
{
	uintptr_t	d_ino;
	off_t	d_off;
	uint16_t d_reclen;
	char d_name[NAME_MAX + 1];
};

#endif
