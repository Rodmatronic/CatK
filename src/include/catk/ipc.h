#ifndef __IPC_H
#define __IPC_H

#define SIGHUP  1
#define SIGINT  2
#define SIGQUIT 3
#define SIGILL  4
#define SIGTRAP 5
#define SIGABRT 6
#define SIGFPE  7
#define SIGKILL 8
#define SIGSEGV 9
#define SIGPIPE 10
#define SIGTERM 11
#define SIGCHLD 12
#define SIGSTOP 13

int dispatch_signal(int signal);

#endif
