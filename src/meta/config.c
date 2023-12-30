#include "config.h"

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
char* bootargs;
char host_name[30] = "";
char username[30] = "";
char* current_directory = "/";
char* versionnumber = "0.061re";
char* vername = "CatK Version Bittin'";
char* arch = "x86-legacy";
char* prebootversion = "0.061re";
char* bootargs = "n/a";