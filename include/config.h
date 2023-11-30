#ifndef CONFIG_CATK_H
#define CONFIG_CATK_H
#include "types.h"

// Structure to represent a user entry
struct User {
    char username[50];
    char shell[50];
};

struct User rootUser;

char* usersh;
char* hostnamesh;
char usersh_buffer[80];
char hostnamesh_buffer[80];
char motdbuffer[1024];
char* hostnamebuffer[128];
char defaulthostname[30] = "catk";
int total_files = 0;
int rows = 0;
extern char* bootargs;


char host_name[30] = ""; // hostname with a maximum length of 63 characters
char username[30] = ""; // username

char* current_directory = "/";  // Set it initially to the root directory

char* versionnumber = "0.059re";
char* vername = "CatK Version Bittin'";
char* arch = "x86_64";
char* prebootversion = "0.059re";
char* bootargs = "n/a"; // Boot args


#endif
