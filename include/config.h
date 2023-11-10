#ifndef CONFIG_CATK_H
#define CONFIG_CATK_H
#include "types.h"

extern uint32 prebootversion;
extern char* bootargs;

char host_name[30] = ""; // hostname with a maximum length of 63 characters
char username[30] = ""; // username

uint32 versionnumber = "0.04.6";
char* vername = "CatK Version Boppin";
uint32 prebootversion = "0.04.6";
char* bootargs = "n/a"; // Boot args

#endif