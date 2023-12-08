#ifndef PREBOOT_H
#define PREBOOT_H
extern char* args;
extern int bootlogo;
extern char* art;
void vga_enable_cursor();
void PreBoot();
#endif