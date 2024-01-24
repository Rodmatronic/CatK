#ifndef CONFIG_CATK_H
#define CONFIG_CATK_H
#include "types.h"

// Structure to represent a user entry
struct User {
    char username[50];
    char shell[50];
};

// Declare the rootUser variable with 'extern'
extern struct User rootUser;

// Declare other variables with 'extern'
extern char* usersh;
extern char* hostnamesh;
extern char usersh_buffer[80];
extern char hostnamesh_buffer[80];
extern char motdbuffer[1024];
extern char* hostnamebuffer[128];
extern char defaulthostname[30];
extern int total_files;
extern int rows;
extern char host_name[30];
extern char username[30];
extern char* current_directory;
extern char* versionnumber;
extern char* vername;
extern char* arch;
//extern char* prebootversion;
extern char input_buffer[1024];
extern int buffer_index;

#endif
