#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

char* strcpy(char* dest, const char* src) {
    char* original_dest = dest;

    while ((*dest++ = *src++) != '\0') {}

    return original_dest;
}