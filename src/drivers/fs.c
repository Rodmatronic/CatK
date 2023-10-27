#include "types.h"

#define MAX_NAME_LENGTH 100

struct File {
    char name[MAX_NAME_LENGTH];
    char extension[MAX_NAME_LENGTH];  // File extension, e.g., ".txt"
    size size;                      // File size in bytes
    int isExecutable;                // Flag to indicate if the file is executable (1 for executable, 0 for non-executable)
};

// Define a struct for folders
struct Folder {
    char name[MAX_NAME_LENGTH];
    struct File files[MAX_NAME_LENGTH];
    int fileCount;
    struct Folder* subfolders[MAX_NAME_LENGTH];
    int subfolderCount;
};

// Define a publicly available current directory string
char currentdir[MAX_NAME_LENGTH];

// Define an "ls" function to list the contents of the current directory
void ls(struct Folder* folder) {
    printf("Contents of %s:\n", folder->name);

    // List subfolders
    printf("Subfolders:\n");
    for (int i = 0; i < folder->subfolderCount; i++) {
        printf("  - %s/\n", folder->subfolders[i]->name);
    }
    if (folder->fileCount > 0) {
        // List files
        printf("Files:\n");
        for (int i = 0; i < folder->fileCount; i++) {
            printf("  - %s\n", folder->files[i].name);
        }
    }
}

// Define a function to initialize the file system
void initfilesystem(struct Folder* root) {
    strcpy(root->name, "/");
    root->fileCount = 0;
    root->subfolderCount = 0;

    // Create subdirectories "bin" and "etc"
    struct Folder bin;
    strcpy(bin.name, "bin");
    bin.fileCount = 0;
    bin.subfolderCount = 0;

    struct Folder etc;
    strcpy(etc.name, "etc");
    etc.fileCount = 0;
    etc.subfolderCount = 0;

    struct Folder var;
    strcpy(var.name, "var");
    var.fileCount = 0;
    var.subfolderCount = 0;

    struct Folder proc;
    strcpy(proc.name, "proc");
    proc.fileCount = 0;
    proc.subfolderCount = 0;

    struct Folder boot;
    strcpy(boot.name, "boot");
    boot.fileCount = 0;
    boot.subfolderCount = 0;

    // Add "bin" and "etc" as subfolders of the root
    root->subfolders[root->subfolderCount++] = &bin;
    root->subfolders[root->subfolderCount++] = &etc;
    root->subfolders[root->subfolderCount++] = &var;
    root->subfolders[root->subfolderCount++] = &proc;
    root->subfolders[root->subfolderCount++] = &boot;

}

// Define a function to create an empty file
void touch(const char* filename, struct Folder* currentDirectory) {
    // Check if a file with the same name already exists in the current directory
    for (int i = 0; i < currentDirectory->fileCount; i++) {
        if (strcmp(currentDirectory->files[i].name, filename) == 0) {
            return;
        }
    }

    // Create a new file
    struct File newFile;
    strcpy(newFile.name, filename);
    newFile.size = 0; // Initialize the file size to 0 since it's an empty file
    newFile.isExecutable = 0; // By default, it's not an executable file

    // Add the new file to the current directory
    currentDirectory->files[currentDirectory->fileCount++] = newFile;
}

// Define a function to create a new directory
void mkdir(const char* dirname) {
    // Create a new folder
    struct Folder newFolder;
    strcpy(newFolder.name, dirname);
    newFolder.fileCount = 0;
    newFolder.subfolderCount = 0;

    // Find the current directory (which is the root in this version)
    root.subfolders[root.subfolderCount++] = &newFolder;
}

