#include <catk/errno.h>
#include <catk/mem.h>
#include <catk/printk.h>
#include <catk/io.h>
#include <catk/types.h>
#include <lib/common.h>
#include <stdint.h>

#define STATUS_REG_THRE BIT(5)

struct serial_dev
{
  uint16_t port;
  bool faulty;
};

// assume that every serial device
// is a good serial device
static struct serial_dev serial_ports[9] =
{
  {0x3f8, false},
  {0x2f8, false},
  {0x3e8, false},
  {0x2e8, false},
  {0x5f8, false},
  {0x4f8, false},
  {0x5e8, false},
  {0x4e8, false},
  {0, true}
};

static int serial_dev_init(uint16_t port)
{
  outb(serial_ports[port].port + 1, 0x00); /* disable interrupts */ 
  outb(serial_ports[port].port + 3, 0x80); /*  */
  outb(serial_ports[port].port, 0x03);    
  outb(serial_ports[port].port + 1, 0x00);
  outb(serial_ports[port].port + 3, 0x03);    
  outb(serial_ports[port].port + 2, 0xc7);
  outb(serial_ports[port].port + 4, 0x0b);
  outb(serial_ports[port].port + 4, 0x1e);
  outb(serial_ports[port].port, '!');
  if(inb(serial_ports[port].port) != '!')
  {
    return -EIO;
  }
  outb(serial_ports[port].port + 4, 0x0f);
  return 0;
}

void serial_init(void)
{
  for(int i = 0; serial_ports[i].port; i++)
  {
    int rc = serial_dev_init(i);
    if(rc < 0)
      serial_ports[i].faulty = true;
  }
}

static inline bool serial_empty_transit(int port)
{
  return inb(serial_ports[port].port + 5) & STATUS_REG_THRE;
}

static int serial_write(int port, const char * str, size_t sz)
{
  if(serial_ports[port].faulty)
    return -EIO;
  for(int i = 0; i < sz; i++)
  {
    while (!serial_empty_transit(port));
    switch(str[i])
    {
      case '\n':
        outb(serial_ports[port].port, '\r');
        outb(serial_ports[port].port, str[i]);
        break;
      default:
        outb(serial_ports[port].port, str[i]);
        break;
    }
  }
  return 0;
}

void debug(const char * fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char * buf = (char *)malloc(256);
  vsprintf(buf, (char *)fmt, ap);
  va_end(ap);
  serial_write(0, buf, strlen(buf));
  free(buf);
}
