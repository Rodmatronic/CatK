#ifndef __LIST_H
#define __LIST_H

#include <catk/mem.h>

struct list {
  void * elem;
  struct list * next;
};

static inline int list_count_elements(struct list * head) {
  int elems = 0;
  struct list * _head = head;
  while(_head) {
    _head = _head->next;
    elems++;
  }
  return elems;
}

static inline struct list * list_node_create(void) {
  return (struct list *)malloc(sizeof(struct list));
}

#endif
