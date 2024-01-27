#include <stdlib.h>

void itoa(char* buf, int base, int num) 
{
    char* p = buf;
    int temp;
    int size = 0;

    // Handle 0 explicitly, otherwise empty string is printed
    if (num == 0) 
    {
        *p++ = '0';
        *p = '\0';
        return;
    }

    // Handle negative numbers for bases other than 10
    if (num < 0 && base != 10) 
    {
        num = -num;
        *p++ = '-';
    }

    // Process individual digits
    while (num != 0) 
    {
        temp = num % base;
        *p++ = (temp < 10) ? temp + '0' : temp + 'a' - 10;
        num = num / base;
        size++;
    }

    *p = '\0';

    // Reverse the string
    for (int i = 0; i < size / 2; i++) 
    {
        char tmp = buf[i];
        buf[i] = buf[size - i - 1];
        buf[size - i - 1] = tmp;
    }
}