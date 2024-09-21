#ifndef __PRINTK_H
#define __PRINTK_H

int printk(const char format[], ...);
void panic(const char format[], ...);
void oops(const char format[], ...);

#endif
