#ifndef __SYSCALL_H
#define __SYSCALL_H

#include <stdint.h>
#include <catk/compiler.h>
#include <catk/types.h>

void sys_exit(int err_code);
int sys_read(int fd, char * buf, size_t sz);
int sys_write(int fd, const char * buf, size_t sz);
int sys_open(const char * filename, int flags, uint16_t mode);
pid_t sys_wait(int * status);
int sys_access(const char * path, int mode);
int sys_chdir(const char * path);
int sys_fork(void);
int sys_execve(const char * pathname, char * const argv[], char * const envp[]);
void sys_beep(int ms);

void syscall_install(void);

#endif
