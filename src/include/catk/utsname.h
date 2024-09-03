#ifndef __UTSNAME_H
#define __UTSNAME_H

#define UTS_SYSNAME "CatK"
#define UTS_NODENAME "meow"
#define UTS_RELEASE "v0.01"
#define UTS_VERSION (__DATE__ " " __TIME__)
#ifdef __i386__
#define UTS_MACHINE "i386"
#else
#define UTS_MACHINE "x86_64"
#endif

#endif
