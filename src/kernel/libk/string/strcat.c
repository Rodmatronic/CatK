#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

char* strcat(char* dest, const char* src) {
    size_t dest_len = strlen(dest);
    size_t i;
    for (i = 0; src[i] != '\0'; ++i) {
        dest[dest_len + i] = src[i];
    }
    dest[dest_len + i] = '\0';
    return dest;
}