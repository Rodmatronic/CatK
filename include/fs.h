#define BLOCK_SIZE 10000
#define FILENAME_SIZE 2048
#define MAX_FILES 1024

// Create an instance of the filesystem
struct FileSystem rootfs;

char buffer[BLOCK_SIZE];