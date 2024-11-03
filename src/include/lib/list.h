#ifndef __LIST_H
#define __LIST_H

#include <catk/mem.h>
#include <lib/common.h>

struct list {
  void * data;
  struct list * next;
  struct list * prev;
};

static inline int list_count_elements(struct list * list) {
  int elems = 0;
  struct list * _head = list;
  while(_head) {
    _head = _head->next;
    elems++;
  }
  return elems;
}

static inline struct list * list_elem_create(void) {
  return (struct list *)malloc(sizeof(struct list));
}

#endif
