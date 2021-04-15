#include "../src/gc.h"
#include <stdio.h>

typedef struct node {
  int data;
  struct node *next;
} node;

typedef node *list;

void init(list *);
void append(list *, int);
void print(list);
int pop(list *);
int length(list);
int list_search(list, int);

int main(int argc, char **argv) {

  printf("TEST5: STANDARD IMPLEMENTATION OF LINKED LIST\n");
  gc_init(argc, argv);

  list l;
  init(&l);

  append(&l, 2);
  append(&l, 4);
  append(&l, 6);
  append(&l, 8);

  pop(&l);
  pop(&l);
  pop(&l);
  // pop(&l);

  print(l);

  gc_run();
  gc_dump();

  printf("\n\n");

  return 0;
}

void init(list *l) {
  *l = NULL;
  return;
}

void append(list *l, int data) {
  node *newnode, *q;
  newnode = (node *)gc_malloc(sizeof(node));
  if (newnode) {
    newnode->data = data;
    newnode->next = NULL;
  } else
    return;

  if (*l == NULL) {
    *l = newnode;
    return;
  }

  q = *l;
  while (q->next)
    q = q->next;
  q->next = newnode;
  return;
}

void print(list l) {
  node *p = l;
  if (p == NULL)
    return;
  printf("%d -> ", p->data);
  p = p->next;
  print(p);
  return;
}

int pop(list *l) {
  node *p = *l;
  if (p == NULL)
    return INT_MIN;
  *l = p->next;
  int d = p->data;
  // free(p);
  // Not freeing here to check if GC works
  return d;
}