#include "../src/gc.h"
#include <stdio.h>

typedef struct hello {
  int a;
  struct hello *next;
} hello;

typedef struct test {
  char a, b, c, d;
} test;

int main(int argc, char **argv) {

  printf("TEST3: TO TEST IF DATA IS RETAINED AFTER COMPACTION\n");
  gc_init(argc, argv);

  hello *a = (hello *)gc_malloc(sizeof(hello));
  a->a = 123;
  a->next = NULL;
  hello *e = (hello *)gc_malloc(sizeof(hello));
  hello *f = (hello *)gc_malloc(sizeof(hello));
  // a->next = e;
  // e->next = f;
  f = NULL;
  e = NULL;
  hello *d = (hello *)gc_malloc(sizeof(hello));
  test *c = (test *)gc_malloc(sizeof(test));
  hello *b = (hello *)gc_malloc(sizeof(hello));
  d->a = 456;
  b->a = 789;
  a->next = d;
  d->next = b;
  b->next = NULL;

  hello *p = a;
  while (p) {
    printf("%p : %d\n", p, p->a);
    p = p->next;
  }

  gc_run();

  gc_dump();

  p = a;
  while (p) {
    printf("%p : %d\n", p, p->a);
    p = p->next;
  }

  printf("\n\n");

  return 0;
}