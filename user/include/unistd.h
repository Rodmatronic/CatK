#ifndef __CATK_UNISTD_H
#define __CATK_UNISTD_H

#include "types.h"

#define NULL ((void *)0)

int write(int fd, const char * buf, size_t count);
int read(int fd, void * buf, size_t count);
int fork(void);
int access(const char * pathname, int mode);
void _exit(int error_code);
int wait(int * status);
int chdir(const char * pathname);
int execv(const char * pathname, char * argv[]);
void beep(int ms);

#endif
