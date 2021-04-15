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

  printf("TEST4: GC_MALLOC MORE MEMORY AFTER COMPACTION\n");
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

  gc_run();

  gc_dump();

  hello *p = (hello *)gc_malloc(sizeof(hello));
  test *q = (test *)gc_malloc(sizeof(test));
  gc_dump();

  printf("\n\n");

  return 0;
}