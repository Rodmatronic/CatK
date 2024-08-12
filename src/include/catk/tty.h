#ifndef __TTY_H
#define __TTY_H

#include <catk/device.h>
#include <catk/termios.h>
#include <catk/console.h>
#include <catk/limits.h>
#include <lib/ring.h>

/* c_cc */
#define VEOF    1
#define VEOL    2
#define VERASE  3
#define VINTR   4
#define VKILL   5
#define VMIN    6
#define VQUIT   7
#define VSTART  8
#define VSTOP   9
#define VSUSP   10
#define VTIME   11
#define VWERASE 12

/* c_iflag */
#define BRKINT  0000001
#define ICRNL   0000002
#define IGNBRK  0000004
#define IGNCR   0000010
#define IGNPAR  0000020
#define INLCR   0000040
#define INPCK   0000100
#define ISTRIP  0000200
#define IUCLC   0000400
#define IXANY   0001000
#define IXOFF   0002000
#define IXON    0004000
#define PARMRK  0010000
/* c_oflag */
#define OPOST   0000001
#define OLCUC   0000002
#define ONLCR   0000004
#define OCRNL   0000010
#define ONOCR   0000020
#define ONLRET  0000040
#define OFILL   0000100
#define OFDEL   0000200
#define NLDLY   0000400
#define   NL0   0000000
#define   NL1   0000400
#define CRDLY   0003000
#define   CR0   0000000
#define   CR1   0001000
#define   CR2   0002000
#define   CR3   0003000
#define TABDLY  0014000
#define   TAB0  0000000
#define   TAB1  0004000
#define   TAB2  0010000
#define   TAB3  0014000
#define BSDLY   0020000
#define   BS0   0000000
#define   BS1   0020000
#define FFDLY   0100000
#define   FF0   0000000
#define   FF1   0100000
#define VTDLY   0040000
#define   VT0   0000000
#define   VT1   0040000
/* c_cflag */
#define CSIZE   0000060
#define   CS5   0000000
#define   CS6   0000020
#define   CS7   0000040
#define   CS8   0000060
#define CSTOPB  0000100
#define CREAD   0000200
#define PARENB  0000400
#define PARODD  0001000
#define HUPCL   0002000
#define CLOCAL  0004000
/* c_lflag */
#define ISIG    0000001
#define ICANON  0000002
#define XCASE   0000004
#define ECHO    0000010
#define ECHOE   0000020
#define ECHOK   0000040
#define ECHONL  0000100
#define ECHOCTL 0001000
#define NOFLSH  0000200
#define TOSTOP  0000400
#define IEXTEN  0001000

#define TTY_BUF_SIZE MAX_CANON

#define NR_CONSOLES 8

#define IS_VALID_TTY(tty) (tty->dev->major == TTYDEV_MAJOR)

struct tty_struct;

/* interface between driver and tty */
struct tty_operations
{
  size_t (*read)(struct tty_struct * tty, uint8_t * buf, size_t count);
  size_t (*write)(struct tty_struct * tty, const uint8_t * buf, size_t count);
};

struct tty_struct
{
  int index;                          /* index of the tty (e.g. tty1, tty2, etc) */
  struct device * dev;                /* device associated with the tty */
  struct termios termios;
  struct winsize winsize;
  struct tty_operations * ops;  /* tty operations */
  struct ring_buffer * read_q;
  struct ring_buffer * write_q;
};

struct tty_struct * tty_lookup(int num);
int tty_create(int num, struct device * dev);
void tty_handle_input(struct tty_struct * tty, int ch);

#endif
