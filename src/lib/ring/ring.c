#include <catk/mem.h>
#include <catk/errno.h>
#include <lib/ring.h>
#include <stdint.h>

int ring_buffer_init(struct ring_buffer * ring, size_t buffer_size)
{
  ring->head = 0;
  ring->tail = 0;
  ring->size = buffer_size;
  ring->buffer = (uint8_t *)malloc(ring->size);
  if(!ring->buffer)
    return -ENOMEM;
  return 0;
}

int ring_buffer_write(struct ring_buffer * ring, uint8_t ch)
{
  if(ring->tail > ring->size)
    ring->tail = 0; 
  ring->buffer[ring->tail++] = ch;
  return ring->tail;
}

int ring_buffer_read(struct ring_buffer * ring, uint8_t * buf, size_t len)
{
  if(ring->head > ring->size)
    ring->head = 0;
  for(int i = 0; i < len; i++)
  {
    *buf = ring->buffer[ring->head++];
    buf++;
  }
  return ring->head;
}

void ring_buffer_release(struct ring_buffer * ring)
{
  if(ring)
  {
    if((void *)ring->buffer)
    {
      free((void *)ring->buffer);
    }
  }
}

static void wait(struct ring_buffer * ring) {
  int prev_tail = ring->tail;
  while(prev_tail == ring->tail) {
    if(ring->tail != prev_tail) {
      break;
    }
    prev_tail = ring->tail;
  }
}

void ring_buffer_read_and_wait(struct ring_buffer * ring, uint8_t * buf, size_t len) {
  for(int i = 0; i < len; i++) {
    wait(ring);
    ring_buffer_read(ring, buf, 1);
  }
}
