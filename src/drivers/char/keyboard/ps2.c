#include <catk/core.h>
#include <catk/device.h>
#include <catk/tty.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/console.h>
#include <catk/io.h>
#include <lib/common.h>

#define KEYBOARD_PORT1_IRQ              33
#define KEYBOARD_PORT2_IRQ              44

#define KEYBOARD_DATA_PORT              0x60
#define KEYBOARD_COMMAND_REG            0x64

#define KEYBOARD_STATUS_OUTPUT          BIT(0) /* must be set before attempting to read data from IO port */
#define KEYBOARD_STATUS_INPUT           BIT(1) /* must be clear before attempting to write data to IO port 0x60 or IO port 0x64 */
#define KEYBOARD_STATUS_SYSTEM          BIT(2) /* Meant to be cleared on reset and set by firmware if the system passes self tests (POST) */
#define KEYBOARD_STATUS_COMMAND         BIT(3) /* 0 = data written to input buffer is data for PS/2 device, 1 = data written to input buffer is data for PS/2 controller command */
#define KEYBOARD_STATUS_TIMEOUT         BIT(6) /* 0 = no error, 1 = time-out error */
#define KEYBOARD_STATUS_PARITY          BIT(7) /* 0 = no error, 1 = parity error */

#define KEYBOARD_CONFIG_BYTE_INTR1      BIT(0)
#define KEYBOARD_CONFIG_BYTE_INTR2      BIT(1)
#define KEYBOARD_CONFIG_BYTE_CLK2       BIT(5)

#define SEND_CMD(cmd)                   outb(KEYBOARD_COMMAND_REG, cmd)
#define READ_CMD                        inb(KEYBOARD_COMMAND_REG)
#define READ_DATA                       inb(KEYBOARD_DATA_PORT)

#define HAS_STATUS(stat)                (READ_CMD & stat)

#define WAIT_FOR_INPUT_STATUS           while(!HAS_STATUS(KEYBOARD_STATUS_INPUT))
#define WAIT_FOR_OUTPUT_STATUS          while(!HAS_STATUS(KEYBOARD_STATUS_OUTPUT))

#define KEYBOARD_CMD_FIRST_ENABLE       0xae
#define KEYBOARD_CMD_FIRST_DISABLE      0xad

#define KEYBOARD_CMD_SECOND_ENABLE      0xa8
#define KEYBOARD_CMD_SECOND_DISABLE     0xa7

#define KEYBOARD_CMD_CONTROLLER_TEST    0xaa

#define KEYBOARD_CMD_FIRST_PORT_TEST    0xab
#define KEYBOARD_CMD_SECOND_PORT_TEST   0xa9

#define KEYBOARD_CMD_CTRL_PORT_READ     0xd0

#define KEYBOARD_CMD_CONFIG_BYTE_READ   0x20
#define KEYBOARD_CMD_CONFIG_BYTE_WRITE  0x60

/* PS/2 controller test results */
#define KEYBOARD_CONTROLLER_TEST_PASS   0x55
#define KEYBOARD_CONTROLLER_TEST_FAIL   0xfc
/* PS/2 keyboard port test results */
#define KEYBOARD_FISRT_PORT_TEST_PASS   0x00
#define KEYBOARD_FIRST_PORT_TEST_CLK_LO 0x01
#define KEYBOARD_FIRST_PORT_TEST_CLK_HI 0x02
#define KEYBOARD_FIRST_PORT_TEST_DAT_LO 0x03
#define KEYBOARD_FIRST_PORT_TEST_DAT_HI 0x04

/* keyboard scancodes */
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

static uint8_t shift_map[256] = {
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

/* assume that the keyboard is single channel */
static bool is_dual_channel = false;
/* keep track of keyboard state (e.g. ctrl key held, alt key held, etc) */
static uint8_t keyboard_flags;

static struct tty_struct * tty = NULL;

static void putc(int ch);

static inline void keyboard_first_enable(void)
{
  SEND_CMD(KEYBOARD_CMD_FIRST_ENABLE);
}

static inline void keyboard_first_disable(void)
{
  SEND_CMD(KEYBOARD_CMD_SECOND_DISABLE);
}

static inline void keyboard_second_enable(void)
{
  SEND_CMD(KEYBOARD_CMD_SECOND_ENABLE);
}

static inline void keyboard_second_disable(void)
{
  SEND_CMD(KEYBOARD_CMD_SECOND_DISABLE);
}

static inline uint8_t keyboard_ctrl_read_out(void)
{
  SEND_CMD(KEYBOARD_CMD_CTRL_PORT_READ);
  return READ_DATA;
}

static inline uint8_t keyboard_ctrl_test(void)
{
  SEND_CMD(KEYBOARD_CMD_CONTROLLER_TEST);
  return READ_DATA;
}

static inline uint8_t keyboard_first_port_test(void)
{
  SEND_CMD(KEYBOARD_CMD_FIRST_PORT_TEST);
  return READ_DATA;
}

static inline uint8_t keyboard_second_port_test(void)
{
  SEND_CMD(KEYBOARD_CMD_SECOND_PORT_TEST);
  return READ_DATA;
}

static inline uint8_t keyboard_ctrl_config_byte_read(void)
{
  SEND_CMD(KEYBOARD_CMD_CONFIG_BYTE_READ);
  return READ_DATA;
}

static inline void keyboard_ctrl_config_byte_write(uint8_t byte)
{
  SEND_CMD(KEYBOARD_CMD_CONFIG_BYTE_WRITE);
  SEND_CMD(byte);
}

static void keyboard_first_intr_flag_set(void)
{
  uint8_t cfg_byte = keyboard_ctrl_config_byte_read();
  if(cfg_byte & KEYBOARD_CONFIG_BYTE_INTR1)
    return;
  cfg_byte |= KEYBOARD_CONFIG_BYTE_INTR1;
  keyboard_ctrl_config_byte_write(cfg_byte);
}

static void keyboard_second_intr_flag_set(void)
{
  uint8_t cfg_byte = keyboard_ctrl_config_byte_read();
  if(cfg_byte & KEYBOARD_CONFIG_BYTE_INTR2)
    return;
  cfg_byte |= KEYBOARD_CONFIG_BYTE_INTR2;
  keyboard_ctrl_config_byte_write(cfg_byte);
}

static void keyboard_test_print_err(uint8_t err)
{
  switch(err)
  {
    case KEYBOARD_FIRST_PORT_TEST_CLK_LO:
    {
      printk("PS2-Port: Error: clock line stuck low\n");
      break;
    }
    case KEYBOARD_FIRST_PORT_TEST_CLK_HI:
    {
      printk("PS2-Port: Error: clock line stuck high\n");
      break;
    }
    case KEYBOARD_FIRST_PORT_TEST_DAT_LO:
    {
      printk("PS2-Port: Error: data line stuck low\n");
      break;
    }
    case KEYBOARD_FIRST_PORT_TEST_DAT_HI:
    {
      printk("PS2-Port: Error: data line stuck high\n");
      break;
    }
    default:
    {
      printk("PS2-Port: Error: received unknown error code: 0x%02x\n", err);
      break;
    }
  }
}

static void keyboard_channel_test(void)
{
  uint8_t cfg_byte;
  SEND_CMD(KEYBOARD_CMD_SECOND_ENABLE);
  cfg_byte = keyboard_ctrl_config_byte_read();
  is_dual_channel = !(cfg_byte & KEYBOARD_CONFIG_BYTE_CLK2);
  printk("PS/2: Keyboard detected as %s\n", is_dual_channel ? "dual-channeled" : "single-channeled");
}

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
    case KEYBOARD_KEY_LSHIFT: /* fall through */
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

static void keyboard_port1_irq(struct intr_stack_frame * frame)
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
  uint8_t * keymap = (keyboard_flags & KEYBOARD_FLAG_SHIFT) ? shift_map : keyboard_map;
  int ch = keymap[scancode & 0x7f];
  if(!(scancode & KEYBOARD_KEYPRESS_STOP))
  {
    /* stores character in tty read queue */
    putc(ch);
  }
}

void keyboard_port2_irq(struct intr_stack_frame * frame)
{

}

/* write string to tty read queue */
static void putc(int ch)
{
  if(!tty)
    return;
  ring_buffer_write(tty->read_q, ch);
}

int keyboard_assign_tty(struct tty_struct * tty_new)
{
  if(!tty_new)
    return -EINVAL;
  if(!tty_new->dev)
    return -ENODEV;
  if(!IS_VALID_TTY(tty_new))
    return -EINVAL;
  tty = tty_new;
  return 0;
}

int keyboard_init(void)
{
  /* disable interrupts */
  critical_enter();
  int rc;
  /* disable so that other ps/2 data ports cant interfere with another */
  keyboard_first_disable();
  keyboard_second_disable();
  rc = keyboard_ctrl_test();
  /* did it pass the test? */
  if(rc == KEYBOARD_CONTROLLER_TEST_FAIL)
  {
    /* not to be confused with the PlayStation 2 :) */
    printk("PS2-Controller: Error: controller test failed\n");
    critical_exit();
    return -EIO;
  }
  rc = keyboard_first_port_test();
  /* did it pass? */
  if(rc != KEYBOARD_FISRT_PORT_TEST_PASS)
  {
    keyboard_test_print_err((uint8_t)rc);
    critical_exit();
    return -EIO;
  }
  keyboard_channel_test();
  /* enable both keyboards */
  keyboard_first_enable();
  if(is_dual_channel)
    keyboard_second_enable();
  /* set interrupt flags on both keyboards */
  keyboard_first_intr_flag_set();
  if(is_dual_channel)
    keyboard_second_intr_flag_set();
  interrupt_install(&keyboard_port1_irq, KEYBOARD_PORT1_IRQ);
  if(is_dual_channel)
    interrupt_install(&keyboard_port2_irq, KEYBOARD_PORT2_IRQ);
  /* by default, we use tty0 */
  tty = tty_lookup(0);
  critical_exit();
  return 0;
}