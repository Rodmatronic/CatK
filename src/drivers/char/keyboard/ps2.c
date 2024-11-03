#include <catk/core.h>
#include <catk/printk.h>
#include <catk/io.h>
#include <catk/tty.h>

/* IO */
#define KEYBOARD_DATA_PORT              0x60
#define KEYBOARD_COMMAND_REG            0x64
#define SEND_CMD(cmd)                   outb(KEYBOARD_COMMAND_REG, cmd)
#define READ_CMD                        inb(KEYBOARD_COMMAND_REG)
#define READ_DATA                       inb(KEYBOARD_DATA_PORT)

#define HAS_STATUS(stat)                (READ_CMD & stat)

#define WAIT_FOR_INPUT_STATUS           while(!HAS_STATUS(KEYBOARD_STATUS_INPUT))
#define WAIT_FOR_OUTPUT_STATUS          while(!HAS_STATUS(KEYBOARD_STATUS_OUTPUT))

/* Status */
#define KEYBOARD_STATUS_OUTPUT          BIT(0) /* must be set before attempting to read data from IO port */
#define KEYBOARD_STATUS_INPUT           BIT(1) /* must be clear before attempting to write data to IO port 0x60 or IO port 0x64 */

/* Keyboard scancodes and flags */
#define KEYBOARD_KEYPRESS_STOP  0x80 /* finger gets lifted off of key */
#define KEYBOARD_KEY_CTRL       0x1d
#define KEYBOARD_KEY_ALT        0x38
#define KEYBOARD_KEY_LSHIFT     0x2a
#define KEYBOARD_KEY_RSHIFT     0x36
#define KEYBOARD_KEY_ESC        0x01

#define KEYBOARD_FLAG_CTRL      BIT(0)  /* 0x01 */
#define KEYBOARD_FLAG_ALT       BIT(1)  /* 0x02 */
#define KEYBOARD_FLAG_SHIFT     BIT(2)  /* 0x04 */
#define KEYBOARD_FLAG_ESC       BIT(3)  /* 0x08 */

static uint8_t keyboard_map[256] = {
  0,  0x1b, '1',  '2',  '3',  '4',  '5',  '6',  // 0x00
  '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
  'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  // 0x10
  'o',  'p',  '[',  ']',  '\n', 0,   'a',  's',
  'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  // 0x20
  '\'', '`',  0,  '\\', 'z',  'x',  'c',  'v',
  'b',  'n',  'm',  ',',  '.',  '/',  0,  '*',  // 0x30
  0,  ' ',  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  '7',  // 0x40
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  0,  0,  0,  0,   // 0x50
  [0x9c] = '\n',      // KP_Enter
  [0xb5] = '/',       // KP_Div
  [0xc8] = 0xe2,      [0xd0] = 0xe3,
  [0xc9] = 0xe6,      [0xd1] = 0xe7,
  [0xcb] = 0xe4,      [0xcd] = 0xe5,
  [0x97] = 0xe0,      [0xcf] = 0xe1,
  [0xd2] = 0xe8,      [0xd3] = 0xe9
};

static uint8_t keyboard_shift_map[256] = {
  0,  033,  '!',  '@',  '#',  '$',  '%',  '^',  // 0x00
  '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
  'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  // 0x10
  'O',  'P',  '{',  '}',  '\n', 0,  'A',  'S',
  'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',  // 0x20
  '"',  '~',  0,  '|',  'Z',  'X',  'C',  'V',
  'B',  'N',  'M',  '<',  '>',  '?',  0,  '*',  // 0x30
  0,  ' ',  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  '7',  // 0x40
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  0,  0,  0,  0,  // 0x50
  [0x9C] = '\n',      // KP_Enter
  [0xB5] = '/',       // KP_Div
  [0xc8] = 0xe2,      [0xd0] = 0xe3,
  [0xc9] = 0xe6,      [0xd1] = 0xe7,
  [0xcb] = 0xe4,      [0xcd] = 0xe5,
  [0x97] = 0xe0,      [0xcf] = 0xe1,
  [0xd2] = 0xe8,      [0xd3] = 0xe9
};

static struct tty_struct * connected_tty = NULL;
static uint8_t keyboard_flags = 0;

static uint8_t keyboard_get_scancode_flag(char scancode)
{
  char ch = scancode & 0x7f; 
  switch(ch)
  {
    case KEYBOARD_KEY_ESC:
    {
      return KEYBOARD_FLAG_ESC;
    }
    case KEYBOARD_KEY_CTRL:
    {
      return KEYBOARD_FLAG_CTRL;
    }
    case KEYBOARD_KEY_ALT:
    {
      return KEYBOARD_FLAG_ALT;
    }
    case KEYBOARD_KEY_LSHIFT:
    {
      /* fall through */
    }
    case KEYBOARD_KEY_RSHIFT:
    {
      return KEYBOARD_FLAG_SHIFT;
    }
    default:
    {
      return 0;
      break;    
    }
  }
}

static void _hot_ keyboard_port1_irq(struct intr_stack_frame * frame)
{
  /* this is required to be set */
  if(!(READ_CMD & KEYBOARD_STATUS_OUTPUT))
    return;
  uint8_t scancode = READ_DATA;
  switch(scancode)
  {
    case 0xff: /* fall through */
    case 0x00:
    {
      printk("PS/2: Keyboard key detection error\n");
      return;
    }
    default:
    {
      break;
    }
  }
  uint8_t flag = keyboard_get_scancode_flag(scancode);
  if(flag)
  {
    /* if key was just released, unset the flag */
    if(scancode & KEYBOARD_KEYPRESS_STOP)
    {
      keyboard_flags &= ~flag;
    }
    else
    {
      /* otherwise, set the flag */
      keyboard_flags |= flag;
    }
    return;
  }
  uint8_t * keymap = (keyboard_flags & KEYBOARD_FLAG_SHIFT) ? keyboard_shift_map : keyboard_map;
  int ch = keymap[scancode & 0x7f];
  if(keyboard_flags & KEYBOARD_FLAG_CTRL) {
    for(int i = 1; i < 12; i++) {
      if(ch == ((connected_tty->termios.c_cc[i] + 32) & 0x7f)) {
        ch = connected_tty->termios.c_cc[i];
      }
    }
  }
  if(!(scancode & KEYBOARD_KEYPRESS_STOP))
  {
    tty_handle_input(connected_tty, ch);
  }
}

void keyboard_init(void)
{
  connected_tty = tty_lookup(0);
  interrupt_install(keyboard_port1_irq, IRQ(1));
}
