#include "string.h"
#include "types.h"
#include "panic.h"

#define MEMORY_POOL_SIZE (6 * 1024 * 1024) // 6 MB in bytes

static char memoryPool[MEMORY_POOL_SIZE];
static char* nextFreeAddress = memoryPool;

// Custom implementation of strrchr
char* k_strrchr(const char* str, char c) {
    char* last_occurrence = NULL;
    while (*str) {
        if (*str == c) {
            last_occurrence = (char*)str;
        }
        str++;
    }
    return last_occurrence;
}

char* k_strstr(const char* haystack, const char* needle) {
    while (*haystack) {
        const char* h = haystack;
        const char* n = needle;
        
        while (*h && *n && (*h == *n || (*h >= 'A' && *h <= 'Z' && *h - 'A' + 'a' == *n))) {
            h++;
            n++;
        }

        if (!*n)
            return (char*)haystack;

        haystack++;
    }

    return NULL;
}

char* k_strchr(const char* str, int c) {
    while (*str != '\0') {
        if (*str == c) {
            return str;
        }
        ++str;
    }

    return NULL;  // Character not found
}

int k_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        ++s1;
        ++s2;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// Custom implementation of strtok for case-sensitive comparison
char* k_strtok(char* str, const char* delim) {
    static char* token = NULL;

    if (str != NULL)
        token = str;

    if (token == NULL)
        return NULL;

    char* start = token;

    while (*token && !k_strchr(delim, *token))
        token++;

    if (*token)
        *token++ = '\0';
    else
        token = NULL;

    return start;
}

char k_strncmp(const char* str1, const char* str2, size n) {
    for (size i = 0; i < n; ++i) {
        if (str1[i] != str2[i]) {
            return (str1[i] < str2[i]) ? -1 : 1;
        }

        if (str1[i] == '\0') {
            return 0;  // Strings are equal up to null terminator
        }
    }

    return 0;  // The first n characters are equal
}

// Custom function to calculate the length of a string with a limit
size strnlen(const char* str, size max_len) {
    size len;
    for (len = 0; len < max_len && str[len] != '\0'; ++len);
    return len;
}

char* strncat(char* dest, const char* src, size n) {
    char* dest_end = dest;
    while (*dest_end != '\0') {
        dest_end++;
    }

    while (*src != '\0' && n > 0) {
        *dest_end++ = *src++;
        n--;
    }

    *dest_end = '\0';

    return dest;
}

int strncmp(const char* str1, const char* str2, size n) {
    for (size i = 0; i < n; ++i) {
        if (str1[i] != str2[i] || str1[i] == '\0' || str2[i] == '\0') {
            return (unsigned char)str1[i] - (unsigned char)str2[i];
        }
    }
    return 0;
}

void* k_malloc(size size) {
    if ((nextFreeAddress + size) <= (memoryPool + MEMORY_POOL_SIZE)) {
        void* allocatedMemory = nextFreeAddress;
        nextFreeAddress += size;
        return allocatedMemory;
    } else {
        // Allocation failed, handle the error
        // For example, you can print an error message or exit the program
        panic("Malloc failed to allocate memory!");
    }
}

int snprintf(char *str, size size, const char *format, const char *arg, ...) {
    int result = 0;

    // Iterate through the format string and copy characters to the buffer
    while (*format && size > 1) {
        if (*format == '%' && *(format + 1) == 's') {
            // Handle %s format specifier
            while (*arg && size > 1) {
                *str++ = *arg++;
                size--;
                result++;
            }
            format += 2;  // Skip %s
        } else {
            *str++ = *format++;
            size--;
            result++;
        }
    }

    // Null-terminate the string
    if (size > 0) {
        *str = '\0';
    }

    return result;
}

int sprintf(char* buffer, const char* format, uint32 value) {
    // Assume a simple format: "%u" for an unsigned integer
    
    // Check if the format string is "%u"
    if (format[0] == '%' && format[1] == 'u' && format[2] == '\0') {
        // Convert the unsigned integer to a string and copy it to the buffer
        int length = 0;
        while (value > 0) {
            buffer[length++] = '0' + (value % 10);
            value /= 10;
        }

        // Reverse the string
        for (int i = 0; i < length / 2; ++i) {
            char temp = buffer[i];
            buffer[i] = buffer[length - i - 1];
            buffer[length - i - 1] = temp;
        }

        // Null-terminate the string
        buffer[length] = '\0';

        return length;
    }

    // Unsupported format
    return -1;
}