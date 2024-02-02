#include <stdlib.h>

int atoi(const char* str) {
    int result = 0;
    int sign = 1;

    // Check for a sign
    if (*str == '-') {
        sign = -1;
        str++;
    }

    // Process each digit in the string
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result * sign;
}