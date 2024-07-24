#include <catk/core.h>
#include <catk/trace.h>
#include <catk/printk.h>
#include <config.h>
#include <stdint.h>

struct stack_frame
{
    struct stack_frame * bp;
    uint32_t ip;
};

void trace_stack(int frames)
{
#if CATK_STACK_TRACE == 1
    struct stack_frame * stack;
    asm volatile("movl %%ebp, %0" : "=r"(stack) ::);
    for(int i = 0; stack && i < frames; ++i)
    {
        printk("frame %d: 0x%08x : [0x%08x]\n", i, stack->ip, stack->bp);
        struct stack_frame * stack = stack->bp;
    }
#endif
}