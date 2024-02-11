#include "config.h"
#include "stdint.h"

char cmdline[256];
char loader[256];
char cpubrand[256];
char cpumodel[256];
uint32_t counter = 0;
uint32_t cpulogicores = 0;
uint64_t seed = 000;
uint64_t mem;
uint64_t bmem;