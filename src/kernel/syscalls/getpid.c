#include <catk/core.h>
#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/task.h>
#include <stdint.h>

pid_t sys_getpid(void) {
    return get_current_task()->pid;
}
