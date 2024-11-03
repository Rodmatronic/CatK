#ifndef __TYPES_H
#define __TYPES_H

#include <stdint.h>

typedef uint16_t  dev_t;   // device numbers
typedef uint16_t  gid_t;   // group ids
typedef uint16_t  mode_t;  // some file attributes
typedef uintptr_t  off_t;   // offset
typedef uint32_t   pid_t;   // process ids
typedef uintptr_t  size_t;  // unsigned long (uint32_t)
typedef intptr_t   ssize_t; // signed long (int32_t)
typedef uint16_t  uid_t;   // user id

#endif
