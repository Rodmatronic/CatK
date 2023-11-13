#ifndef CONFIG_CATK_H
#define CONFIG_CATK_H
#include "types.h"

int rows = 0;

extern uint32 prebootversion;
extern char* bootargs;

char host_name[30] = ""; // hostname with a maximum length of 63 characters
char username[30] = ""; // username

uint32 versionnumber = "0.05";
char* vername = "CatK Version Boppin";
uint32 prebootversion = "0.05";
char* bootargs = "n/a"; // Boot args

#endif
