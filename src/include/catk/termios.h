#ifndef __TERMIOS_H
#define __TERMIOS_H

#include <stdint.h>

typedef uint8_t   cc_t;
typedef uint32_t  tcflag_t;

struct winsize
{
  uint16_t ws_row;
  uint16_t ws_col;
  uint16_t ws_xpixel;
  uint16_t ws_ypixel;
};

#define NCCS 19

struct termios
{
	tcflag_t c_iflag;		/* input mode flags */
	tcflag_t c_oflag;		/* output mode flags */
	tcflag_t c_cflag;		/* control mode flags */
	tcflag_t c_lflag;		/* local mode flags */
	cc_t c_line;			  /* line discipline */
	cc_t c_cc[NCCS];		/* control characters */
};

#endif
