#include "fs.h"
#include "console.h"
#include "libc.h"
#include "read.h"
#include "config.h"
#include "kernel.h"
#include "vga.h"
#include "launchp.h"
#include "bitmap.h"
#include "panic.h"
#include "keyboard.h"
#include "syspw.h"
#include "time.h"

char* app = "CatK vga interface";
enum vga_color back_color = BLUE;

void drawconfirmwindow(char* name, char* text, char* textp2, char* textp3)
{
    vga_graphics_draw_rect(VGA_MAX_WIDTH/5, VGA_MAX_HEIGHT/5, VGA_MAX_WIDTH/2, VGA_MAX_HEIGHT/3, BLACK);
    vga_graphics_draw_rect(VGA_MAX_WIDTH/5 + 1, VGA_MAX_HEIGHT/5 + 1, VGA_MAX_WIDTH/2, VGA_MAX_HEIGHT/3, BLACK);
    vga_graphics_fill_rect(VGA_MAX_WIDTH/5 + 1, VGA_MAX_HEIGHT/5 + 1, VGA_MAX_WIDTH/2 - 2, VGA_MAX_HEIGHT/3 - 2, GREY);
    vga_graphics_draw_rect(VGA_MAX_WIDTH/5, VGA_MAX_HEIGHT/5, VGA_MAX_WIDTH/2, VGA_MAX_HEIGHT/12, BLACK);
    draw_string(VGA_MAX_WIDTH/5 + 2, VGA_MAX_HEIGHT/5 + 4, COLOR_DARK_GREY, name);
    draw_string(VGA_MAX_WIDTH/5 + 2, VGA_MAX_HEIGHT/5 + 20, COLOR_DARK_GREY, text);
    draw_string(VGA_MAX_WIDTH/5 + 2, VGA_MAX_HEIGHT/5 + 30, COLOR_DARK_GREY, textp2);
    draw_string(VGA_MAX_WIDTH/5 + 2, VGA_MAX_HEIGHT/5 + 40, COLOR_DARK_GREY, textp3);
    draw_string(VGA_MAX_WIDTH/5 + 2, VGA_MAX_HEIGHT/5 + 55, COLOR_DARK_GREY, "1  ok");
    draw_string(VGA_MAX_WIDTH/5 + 80, VGA_MAX_HEIGHT/5 + 55, COLOR_DARK_GREY, "2  back");
}

void drawpopupwindow()
{
    vga_graphics_draw_rect(VGA_MAX_WIDTH/4, VGA_MAX_HEIGHT/4, VGA_MAX_WIDTH/2, VGA_MAX_HEIGHT/2, BLACK);
    vga_graphics_draw_rect(VGA_MAX_WIDTH/4 + 1, VGA_MAX_HEIGHT/4 + 1, VGA_MAX_WIDTH/2, VGA_MAX_HEIGHT/2, BLACK);
    vga_graphics_fill_rect(VGA_MAX_WIDTH/4 + 1, VGA_MAX_HEIGHT/4 + 1, VGA_MAX_WIDTH/2 - 2, VGA_MAX_HEIGHT/2 - 2, GREY);
    vga_graphics_draw_rect(VGA_MAX_WIDTH/4, VGA_MAX_HEIGHT/4, VGA_MAX_WIDTH/2, VGA_MAX_HEIGHT/12, BLACK);
}

void constUI()
{
    // Menu bar
    vga_graphics_draw_rect(0, 0, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 2, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 4, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 6, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 8, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 10, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 11, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 13, VGA_MAX_WIDTH-1, 0, BLACK);
    draw_string(1, 3, COLOR_DARK_GREY, app);

    // Taskbar
    vga_graphics_draw_rect(0, 14, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 16, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 18, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 20, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 22, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 24, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 26, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 28, VGA_MAX_WIDTH-1, 0, BLACK);
    draw_string(1, 17, COLOR_DARK_GREY, "About");
    draw_string(50, 17, COLOR_DARK_GREY, "Control");
    draw_string(110, 17, COLOR_DARK_GREY, "leave");
    draw_string(160, 17, COLOR_DARK_GREY, "power");
    vga_graphics_draw_rect(0, 26, VGA_MAX_WIDTH-1, 1, GREY);
}

void loginapp()
{
    app = "login";
    while (1)
    {
        //constUI();
        // Read a key scancode
        unsigned char scancode = read_key();

        // Convert the scancode to a character
        char key = scancode_to_char(scancode);

        if (scancode == 0x0E) {
            loginapp();
            break;
        }

        if (scancode == 0x02) {
            vga_graphics_interface();
            break;
        }

        drawpopupwindow();
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 4, COLOR_DARK_GREY, "login ");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 20, COLOR_DARK_GREY, "users:");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 40, COLOR_DARK_GREY, "1  root");

    }
}

void sessionapp()
{
    app = "session";
    while (1)
    {
        constUI();
        // Read a key scancode
        unsigned char scancode = read_key();

        // Convert the scancode to a character
        char key = scancode_to_char(scancode);

        if (scancode == 0x0E) {
            vga_graphics_interface();
            break;
        }

        if (scancode == 0x02) {
            vga_graphics_interface();
            break;
        }

        if (scancode == 0x03) {
            vga_graphics_exit();
            vga_graphics_init();
            loginapp();
            break;
        }

        drawpopupwindow();
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 4, COLOR_DARK_GREY, "session ");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 20, COLOR_DARK_GREY, "Warning this will");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 30, COLOR_DARK_GREY, "end your catk");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 40, COLOR_DARK_GREY, "session");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 55, COLOR_DARK_GREY, "1  Back");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 65, COLOR_DARK_GREY, "2  End");

    }
}

void controlpanelapp()
{
    app = "control panel";
    while (1)
    {
        constUI();
        // Read a key scancode
        unsigned char scancode = read_key();

        // Convert the scancode to a character
        char key = scancode_to_char(scancode);

        if (scancode == 0x0E) {
            vga_graphics_interface();
            break;
        }
        if (scancode == 0x02) {
            back_color--;
            vga_graphics_clear_color(back_color);

        }
        if (scancode == 0x03) {
            back_color++;
            vga_graphics_clear_color(back_color);
        }

        drawpopupwindow();
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 4, COLOR_DARK_GREY, "Control panel ");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 20, COLOR_DARK_GREY, "background color ");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 32, COLOR_DARK_GREY, "Cycle using 1 and 2 ");

        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 90, COLOR_DARK_GREY, "2023-catk");
    }
}

void about()
{
    app = "about";
    char* workingdir = current_directory;
    current_directory = "/etc";
    read_from_file(&rootfs, "session", buffer2, sizeof(buffer2), 0);
    read_from_file(&rootfs, "hostname", buffer4, sizeof(buffer4), 0);
    current_directory = "/proc";
    read_from_file(&rootfs, "versionnum", buffer, sizeof(buffer), 0);
    read_from_file(&rootfs, "arch", buffer3, sizeof(buffer3), 0);
    read_from_file(&rootfs, "ostype", buffer5, sizeof(buffer5), 0);
    write_serial("uname: ");
    write_serial(buffer);
    write_serial("\n");
    current_directory = workingdir;
    while (1)
    {
        constUI();
        // Read a key scancode
        unsigned char scancode = read_key();

        // Convert the scancode to a character
        char key = scancode_to_char(scancode);

        if (scancode == 0x0E) {
            vga_graphics_interface();
            break;
        }

        drawpopupwindow();
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 4, COLOR_DARK_GREY, "About catk ");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 20, COLOR_DARK_GREY, "version ");
        draw_string(VGA_MAX_WIDTH/4 + 75, VGA_MAX_HEIGHT/4 + 20, COLOR_DARK_GREY, buffer);

        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 32, COLOR_DARK_GREY, "session ");
        draw_string(VGA_MAX_WIDTH/4 + 75, VGA_MAX_HEIGHT/4 + 32, COLOR_DARK_GREY, buffer2);

        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 42, COLOR_DARK_GREY, "arch ");
        draw_string(VGA_MAX_WIDTH/4 + 75, VGA_MAX_HEIGHT/4 + 42, COLOR_DARK_GREY, buffer3);

        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 52, COLOR_DARK_GREY, "hostname ");
        draw_string(VGA_MAX_WIDTH/4 + 75, VGA_MAX_HEIGHT/4 + 52, COLOR_DARK_GREY, buffer4);

        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 62, COLOR_DARK_GREY, "type ");
        draw_string(VGA_MAX_WIDTH/4 + 75, VGA_MAX_HEIGHT/4 + 62, COLOR_DARK_GREY, buffer5);

        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 90, COLOR_DARK_GREY, "2023-catk");
    }
}

void powerapp()
{
    app = "power";
    while (1)
    {
        constUI();
        // Read a key scancode
        unsigned char scancode = read_key();

        // Convert the scancode to a character
        char key = scancode_to_char(scancode);

        if (scancode == 0x0E) {
            vga_graphics_interface();
            break;
        }

        if (scancode == 0x02) {
            vga_graphics_interface();
            break;
        }

        if (scancode == 0x03) {
            drawconfirmwindow("Warning", "this will end", "your catk session", "");
            while (1)
            {
                unsigned char scancode = read_key();
                char key = scancode_to_char(scancode);

                if (scancode == 0x03) {
                    vga_graphics_clear_color(back_color);
                    powerapp();
                    break;
                }

                if (scancode == 0x02) {
                    break;
                }
            }
            vga_graphics_exit();
            vga_graphics_clear_color(BLACK);
            syspw(1);
            break;
        }

        if (scancode == 0x04) {
            drawconfirmwindow("Warning", "this will end", "your catk session", "");
            while (1)
            {
                unsigned char scancode = read_key();
                char key = scancode_to_char(scancode);

                if (scancode == 0x03) {
                    vga_graphics_clear_color(back_color);
                    powerapp();
                    break;
                }

                if (scancode == 0x02) {
                    break;
                }
            }
            syspw(0);
            break;
        }

        if (scancode == 0x05) {
            drawconfirmwindow("Warning", "this will end", "your catk session", "");
            while (1)
            {
                unsigned char scancode = read_key();
                char key = scancode_to_char(scancode);

                if (scancode == 0x03) {
                    vga_graphics_clear_color(back_color);
                    powerapp();                    
                    break;
                }

                if (scancode == 0x02) {
                    break;
                }
            }
            vga_graphics_exit();
            syspw(2);
            break;
        }

        if (scancode == 0x06) {
            drawconfirmwindow("Warning", "this will end", "your catk session", "");
            while (1)
            {
                unsigned char scancode = read_key();
                char key = scancode_to_char(scancode);

                if (scancode == 0x03) {
                    vga_graphics_clear_color(back_color);
                    powerapp();                    
                    break;
                }

                if (scancode == 0x02) {
                    break;
                }
            }
            panic("Triggered by user");
        }

        drawpopupwindow();
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 4, COLOR_DARK_GREY, "Power");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 20, COLOR_DARK_GREY, "1  Back");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 32, COLOR_DARK_GREY, "2  Shutdown");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 42, COLOR_DARK_GREY, "3  Reboot");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 52, COLOR_DARK_GREY, "4  Halt");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 62, COLOR_DARK_GREY, "5  Panic");
    }
}

void vga_graphics_interface()
{   
    app = "desktop";

    while (1)
    {
        constUI();
        // Read a key scancode
        unsigned char scancode = read_key();

        // Convert the scancode to a character
        char key = scancode_to_char(scancode);

        if (scancode == 0x02)
        {
            about();
        }
        if (scancode == 0x03)
        {
            controlpanelapp();
        }
        if (scancode == 0x04)
        {
            sessionapp();
        }

        if (scancode == 0x05)
        {
            powerapp();
        }

        draw_string(1, 17, COLOR_DARK_GREY, key);

        vga_graphics_clear_color(back_color);

    }
}
