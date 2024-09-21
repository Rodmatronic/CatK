#include <catk/printk.h>
#include <catk/virt.h>
#include <catk/console.h>
#include <catk/io.h>
#include <catk/platform.h>
#include <lib/common.h>
#include <lib/ctype.h>
#include <stdint.h>
#include <config.h>
#include <catk/keyb.h>

// PS/2 KEYBOARD DRIVERS
// Based on pritamzope's PS/2 Driver code

static int g_caps_lock = 0;
static int g_shift_pressed = 0;
char g_ch = 0, g_scan_code = 0;

char upper(char c) {
    if ((c >= 'a') && (c <= 'z'))
        return (c - 32);
    return c;
}

// see scan codes defined in keyboard.h for index
char g_scan_code_chars[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int get_scancode() {
    int i, scancode = 0;

    // get scancode until status is on(key pressed)
    for (i = 1000; i > 0; i++) {
        // Check if scan code is ready
        if ((inb(KEYBOARD_STATUS_PORT) & 1) == 0) continue;
        // Get the scan code
        scancode = inb(KEYBOARD_DATA_PORT);
        break;
    }
    if (i > 0)
        return scancode;
    return 0;
}

char alternate_chars(char ch) {
    switch(ch) {
        case '`': return '~';
        case '1': return '!';
        case '2': return '@';
        case '3': return '#';
        case '4': return '$';
        case '5': return '%';
        case '6': return '^';
        case '7': return '&';
        case '8': return '*';
        case '9': return '(';
        case '0': return ')';
        case '-': return '_';
        case '=': return '+';
        case '[': return '{';
        case ']': return '}';
        case '\\': return '|';
        case ';': return ':';
        case '\'': return '\"';
        case ',': return '<';
        case '.': return '>';
        case '/': return '?';
        default: return ch;
    }
}

void keyboard_handler(struct intr_stack_frame *frame) {
    int scancode;
    g_ch = 0;
    scancode = get_scancode();
    g_scan_code = scancode;

    if (scancode & 0x80) {
        // key release
        scancode &= 0x7F;
        switch(scancode) {
            case SCAN_CODE_KEY_LEFT_SHIFT:
            case SCAN_CODE_KEY_RIGHT_SHIFT:
                g_shift_pressed = 0;
                break;
        }
    } else {
        // key down
        switch(scancode) {
            case SCAN_CODE_KEY_CAPS_LOCK:
                g_caps_lock = !g_caps_lock;
                break;

            case SCAN_CODE_KEY_ENTER:
                g_ch = '\n';
                printk("\n");
                break;

            case SCAN_CODE_KEY_TAB:
                g_ch = '\t';
                printk("\t");
                break;

            case SCAN_CODE_KEY_LEFT_SHIFT:
            case SCAN_CODE_KEY_RIGHT_SHIFT:
                g_shift_pressed = 1;
                break;

            default:
                g_ch = g_scan_code_chars[scancode];
                if (g_caps_lock) {
                    if (g_shift_pressed) {
                        g_ch = alternate_chars(g_ch);
                    } else {
                        g_ch = upper(g_ch);
                    }
                } else {
                    if (g_shift_pressed) {
                        if (isalpha(g_ch))
                            g_ch = upper(g_ch);
                        else 
                            g_ch = alternate_chars(g_ch);
                    } else {
                        g_ch = g_scan_code_chars[scancode];
                    }
                }
                printk("%c", g_ch);
                break;
        }
    }
}


void keyb_init() {
    int IRQ = 0x21;
    printk("keyb init - IRQ %x\n", IRQ);
    intr_add_handler(IRQ, keyboard_handler);
}

// a blocking character read
char kb_getchar() {
    char c;

    while(g_ch <= 0);
    c = g_ch;
    g_ch = 0;
    g_scan_code = 0;
    return c;
}

char kb_get_scancode() {
    char code;

    while(g_scan_code <= 0);
    code = g_scan_code;
    g_ch = 0;
    g_scan_code = 0;
    return code;
}

