#ifndef __CATK_UNISTD_H
#define __CATK_UNISTD_H

#include <stdint.h>

#define NULL ((void *)0)

int write(int fd, const char * buf, int count);
int read(int fd, void * buf, int count);
int open(const char * pathname, int flags, uint16_t mode);
int fork(void);
int access(const char * pathname, int mode);
void _exit(int error_code);
int wait(int * status);
int chdir(const char * pathname);
int execv(const char * pathname, char * argv[]);
void beep(int ms);

#endif
