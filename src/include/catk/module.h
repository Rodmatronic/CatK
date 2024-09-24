#ifndef __MODULE_H
#define __MODULE_H

<<<<<<< HEAD
#include <catk/compiler.h>

struct module {
  const char * name;
  const char * author;
  int (*init)(void);
};

#define MODULE_NAME(name) static const char _unused_ __module_name__[256] = name
#define MODULE_AUTHOR(author) static const char _unused_ __module_auth__[256] = author;

#define MODULE_START _MODULE_START(__module_name__, __module_auth__)
#define _MODULE_START(mod_name, auth) static struct module __module_##mod_name__ _used_ _section(".mod_entries") = { .name = mod_name, .author = auth,

#define MODULE_END };

#define module_print(...) _module_print(__module_name__, __VA_ARGS__);

void _module_print(const char * mod_name, const char * fmt, ...);
void builtin_modules_init(void);
=======
#define DEFINE_MODULE(name) static const char * __module_name__ = name
#define MODULE_NAME __module_name__
>>>>>>> 6be3bbb619dded66460b24ba64e5925e2bee774a

#endif
