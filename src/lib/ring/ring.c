#include <catk/mem.h>
#include <catk/errno.h>
#include <catk/debug.h>
#include <lib/ring.h>
#include <stdint.h>

int ring_buffer_init(struct ring_buffer * ring, size_t buffer_size)
{
  ring->head = 0;
  ring->tail = 0;
  ring->size = buffer_size;
  ring->buffer = (uint8_t *)malloc(ring->size);
  ring->count = 0;
  if(!ring->buffer)
    return -ENOMEM;
  return 0;
}

int ring_buffer_write(struct ring_buffer * ring, uint8_t ch)
{
  if(ring->count == ring->size) {
    return -1;
  }
  ring->tail %= ring->size;
  ring->buffer[ring->tail++] = ch;
  ring->count++;
  //debug("WRITE: ring->count: %d\n", ring->count);
  return ring->tail;
}

int ring_buffer_read(struct ring_buffer * ring, uint8_t * buf, size_t len)
{
  if(ring->count == 0) {
    return -1;
  }
  ring->head %= ring->size;
  for(size_t i = 0; i < len; i++)
  {
    if(!ring->count) {
      break;
    }
    *buf = ring->buffer[ring->head++];
    buf++;
    ring->count--;
    //debug("READ: ring->count: %d, ring->head: %d\n", ring->count, ring->head);
  }
  return ring->head;
}

int ring_buffer_read_single(struct ring_buffer * ring, uint8_t * data) {
  if(ring->count == 0) {
    return -1;
  }
  ring->head %= ring->size;
  *data = ring->buffer[ring->head++];
  ring->count--;
  return ring->head;
}

void ring_buffer_clear(struct ring_buffer * ring) {
  memset(ring->buffer, 0, ring->size);
  ring->head = 0;
  ring->tail = 0;
  ring->count = 0;
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
