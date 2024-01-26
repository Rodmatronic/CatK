#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "term.h"
 
void kmain() 
{
	/* Initialize terminal interface */
	terminal_initialize();
 
	/* Newline support is left as an exercise. */
	terminal_writestring("Cat... kernel!\n");
}