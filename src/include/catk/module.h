#ifndef __MODULE_H
#define __MODULE_H

#define DEFINE_MODULE(name) static const char * __module_name__ = name
#define MODULE_NAME __module_name__

#endif
