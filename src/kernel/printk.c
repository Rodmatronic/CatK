#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "term.h"
#include "libc.h"

void itoa_hex(char* buf, int num) 
{
    // Assuming a 32-bit integer
    for (int i = 7; i >= 0; --i) 
    {
        int nibble = (num >> (i * 4)) & 0xF;
        buf[7 - i] = (nibble < 10) ? nibble + '0' : nibble - 10 + 'a';
    }
    buf[8] = '\0';
}

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
                terminal_write_int(num);
                arg += sizeof(int);
            } 
            else if (*format == 'x') 
            {
                int num = *((int*)arg);
                char hex_buf[9];
                itoa_hex(hex_buf, num);
                terminal_write(hex_buf, strlen(hex_buf));
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
