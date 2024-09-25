#ifndef __MODULE_H
#define __MODULE_H

#include <catk/compiler.h>
#include <catk/kernel.h>

struct module {
  const char * name;
  const char * author;
  int (*init)(void);
  void (*exit)(void);
};

#define NO_LOADED_MODULES (&kmod_start - &kmod_end == 0)

#define MODULE_NAME(name) static const char _unused_ __module_name__[32] = name
#define MODULE_AUTHOR(author) static const char _unused_ __module_auth__[64] = author;

#define module_init(initcall, exitcall) _MODULE_START(__module_name__, __module_auth__, initcall, exitcall)
#define _MODULE_START(mod_name, auth, func1, func2) static struct module __module_##mod_name__ _used_ _section(".mod_entries") = { .name = mod_name, .author = auth, .init = func1, .exit = func2 }

#define module_print(...) _module_print(__module_name__, __VA_ARGS__);

void _module_print(const char * mod_name, const char * fmt, ...);
void builtin_modules_init(void);

#endif
