#ifndef GARBAGE_COLLECTOR_LIST_H_
#define GARBAGE_COLLECTOR_LIST_H_
#include "gc.h"

typedef struct dlist {
  collectorBlock *head, *tail;
} dlist;

void init_list(dlist *);
int length(dlist);
void insert_beginning(dlist *, int);
void insert_at(dlist *, int data, int index);
void append(dlist *, int);
int remove_first(dlist *);
int remove_at(dlist *, int);
int remove_last(dlist *);
void traverse_from_start(dlist);
void traverse_from_tail(dlist);
void destroy_list(dlist *);
int search(dlist);

#endif
