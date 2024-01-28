#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <term.h>
#include <pc.h>
#include <string.h>

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_init(void) 
{
    vga_set_cursor_pos(0, 0);
    vga_enable_cursor();
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_clear() 
{
    for (size_t y = 0; y < VGA_HEIGHT; y++) 
    {
        for (size_t x = 0; x < VGA_WIDTH; x++) 
        {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
    
    terminal_row = 0;
    terminal_column = 0;
    vga_set_cursor_pos(0, 0);
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void vga_set_cursor_pos(uint8_t x, uint8_t y) 
{
    uint16_t cursorLocation = y * VGA_WIDTH + x;
    outportb(0x3D4, 14);
    outportb(0x3D5, cursorLocation >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, cursorLocation);
}


void terminal_putchar(char c) 
{
    if (c == '\n') 
    {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
        {
            terminal_row--;
            vga_set_cursor_pos(0, 24);
            // Scroll up when the screen is full
            for (size_t y = 0; y < VGA_HEIGHT - 1; y++) 
            {
                for (size_t x = 0; x < VGA_WIDTH; x++) 
                {
                    const size_t index = y * VGA_WIDTH + x;
                    const size_t nextIndex = (y + 1) * VGA_WIDTH + x;
                    terminal_buffer[index] = terminal_buffer[nextIndex];
                }
            }

            // Clear the last line
            for (size_t x = 0; x < VGA_WIDTH; x++) 
            {
                const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
                terminal_buffer[index] = vga_entry(' ', terminal_color);
            }
        }
    } 
    else 
    {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

        if (++terminal_column == VGA_WIDTH) 
        {
            for (size_t y = 0; y < VGA_HEIGHT - 1; y++) 
            {
                for (size_t x = 0; x < VGA_WIDTH; x++) 
                {
                    const size_t index = y * VGA_WIDTH + x;
                    const size_t nextIndex = (y + 1) * VGA_WIDTH + x;
                    terminal_buffer[index] = terminal_buffer[nextIndex];
                }
            }

            // Clear the last line
            for (size_t x = 0; x < VGA_WIDTH; x++) 
            {
                const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
                terminal_buffer[index] = vga_entry(' ', terminal_color);
            }
        }

        // Update the cursor position using vga_set_cursor_pos
        vga_set_cursor_pos(terminal_column, terminal_row);
    }
}

void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_write_int() 
{
    char buf[20]; // Assuming a 32-bit integer
    terminal_write(buf, strlen(buf));
}

void vga_enable_cursor() {
    outportb(0x3D4, 0x0A);
    outportb(0x3D5, (inportb(0x3D5) & 0xC0) | 0);
    outportb(0x3D4, 0x0B);
    outportb(0x3D5, (inportb(0x3D5) & 0xE0) | 0x0E);
}