#include "config.h"
#include "stdint.h"



/*
 *    If you are looking for the configuration file, it is in 𝗰𝗼𝗻𝗳𝗶𝗴.𝗵
 *        This file just exists so that clang doesn't complain.
 */



char cmdline[256];
char loader[256];
char cpubrand[256];
char cpumodel[256];
char pwd[1024];

//------------------------------------------------

int ttyno = 12;
int currentttysession = 1;
uint32_t counter = 0;
uint32_t cpulogicores = 0;
uint64_t seed = 000;
uint64_t mem;
uint64_t memreal;
uint64_t bmem;