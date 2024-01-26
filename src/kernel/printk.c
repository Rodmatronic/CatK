#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <term.h>
#include "term.h"
#include "libc.h"

void printk(const char* format, ...) 
{
    // Pointer to the first variable argument
    const char* arg = (const char*)(&format + 1);

    // Iterate through the format string
    while (*format != '\0') 
    {
        // Check for format specifiers
        if (*format == '%') 
        {
            ++format; // Move past '%'

            // Handle different specifiers
            if (*format == 's') 
            {
                const char* str = *((const char**)arg);
                terminal_write(str, strlen(str));
                arg += sizeof(const char*);
            } 
            else if (*format == 'd') 
            {
                int num = *((int*)arg);
                terminal_write(num, sizeof(num));
                arg += sizeof(int);
            } 
            else if (*format == 'C') 
            {
                // Update the colors based on the arguments
                uint8_t color = *((uint8_t*)arg);
                terminal_setcolor(color);
                arg += sizeof(uint8_t);
            }
            
            // Move to the next character in the format string
            ++format;
        } 
        else 
        {
            terminal_write(format, 1);
            ++format;
        }
    }
}
