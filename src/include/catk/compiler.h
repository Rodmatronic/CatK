#ifndef __COMPILER_H
#define __COMPILER_H

#define _cold_        __attribute__((cold))         /* unlikely to be called */
#define _hot_        __attribute__((hot))           /* likely to be called */
#define _noreturn_    __attribute__((noreturn))     /* should not return */
#define _packed_      __attribute__((packed))       /* little to no alignment */
#define _naked_       __attribute__((naked))        /* doesnt set up stack frame */
#define _aligned(x)    __attribute__((aligned(x)))  /* align something by x bytes */
#define unreachable   __builtin_unreachable()       /* code shouldn't be reached */
#define _weak_        __attribute__((weak))         /* linker wont check for definition of something */
#define _unused_      __attribute__((unused))       /* variable is unused current file */

#endif
