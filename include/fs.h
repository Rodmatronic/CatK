#define BLOCK_SIZE 100000
#define FILENAME_SIZE 10
#define MAX_FILES 1024

// Create an instance of the filesystem
struct FileSystem rootfs;

char buffer[BLOCK_SIZE];