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
char host_name[30] = "catk";
char username[30] = "root";
char* current_directory = "/";
char* versionnumber = "0.068";
char* vername = "CatK Version Bittin'";
char* arch = "x86";
//char* prebootversion = "0.066re";
int buffer_index = 0;
char input_buffer[1024];
