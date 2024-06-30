#ifndef __COMPILER_H
#define __COMPILER_H

#define _cold_        __attribute__((cold))
#define _noreturn_    __attribute__((noreturn))
#define _packed_      __attribute__((packed))
#define _naked_       __attribute__((naked))

#define unreachable   __builtin_unreachable()

#endif
