#ifndef __SYSCALL_H
#define __SYSCALL_H

#include <stdint.h>
#include <catk/compiler.h>
#include <catk/types.h>

void sys_exit(int err_code);
int sys_read(int fd, char * buf, size_t sz);
int sys_open(const char * filename, int flags, uint16_t mode);
void syscall_install(void);

#endif
