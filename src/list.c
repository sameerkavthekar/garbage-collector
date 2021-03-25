#include "list.h"

void init_list(dlist *d) {
  d->head = NULL;
  d->tail = NULL;
}

int length(dlist l) {
  collectorBlock *p = l.head;
  int length = 0;
  while (p) {
    length++;
    p = p->next;
  }
  return length;
}

void insert_beginning(dlist *l, collectorBlock c) {
  collectorBlock *nn = (collectorBlock *)malloc(sizeof(collectorBlock));
  collectorBlock *p = l->head;
  if (!nn)
    return;
  else {
    nn->data = data;
    nn->next = NULL;
    nn->prev = NULL;
  }
  if (l->head == NULL) {
    l->head = nn;
    l->tail = nn;
  }
  nn->next = l->head;
  l->head = nn;
}
