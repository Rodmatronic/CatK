#ifndef EXEC_H
#define EXEC_H

#include "fs.h"
#include "types.h"

void execute_file(struct FileSystem* fs, const char* filename);

#endif