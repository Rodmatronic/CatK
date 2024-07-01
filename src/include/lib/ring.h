#ifndef __RING_H
#define __RING_H

#include <catk/types.h>
#include <stdint.h>

struct ring_buffer
{
  int head; // (write)
  int tail; // (read)
  volatile size_t size;
  uint8_t * buffer;
};

int ring_buffer_init(struct ring_buffer * ring, size_t buffer_size);
int ring_buffer_write(struct ring_buffer * ring, uint8_t ch);
int ring_buffer_read(struct ring_buffer * ring, uint8_t * buf, size_t len);
void ring_buffer_release(struct ring_buffer * ring);

#endif
