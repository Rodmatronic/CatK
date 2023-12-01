#ifndef EXEC_H
#define EXEC_H

#include "types.h"
#include "../src/drivers/exec.c"

void execute_file(struct FileSystem* fs, const char* filename, int quiet);

#endif