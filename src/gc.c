#include "gc.h"

typedef struct hello {
  int a;
  struct hello *next;
} hello;

typedef struct test {
  char a, b, c, d;
} test;

// TODO: IMPLEMENT A VECTOR TO HOLD ALL ROOTS


void init() {
  GC.frees = 0;
  GC.mallocs = 0;
  GC.stack_top = __builtin_frame_address(1);
  GC.heap_top = sbrk(0);
  GC.alloc = NULL;
  GC.free = NULL;
  GC.bytes_alloc = 0;
  GC.blocks_alloc = 0;
  init_set(&(GC.addresses));
}

void *gc_malloc(int size) {
  void *block = (void *)malloc(sizeof(collectorBlock) + size);
  // if (!block) {
  //   gc_run();
  //   void *block = (void *)malloc(sizeof(collectorBlock) + size);
  //   if(!block) {
  //     printf("Out of Memory\n");
  //     exit(0);
  //   }
  // }

  collectorBlock node;
  node.next = NULL;
  node.prev = NULL;
  node.size = size;
  node.free = '0';

  memcpy(block, &node, sizeof(collectorBlock));

  if (!block)
    return NULL;

  insert(&(GC.addresses), (uintptr_t *)(block + sizeof(collectorBlock)));

  if (!GC.alloc) {
    GC.alloc = block;
    GC.blocks_alloc++;
    GC.bytes_alloc += sizeof(block) + size;
    return block + sizeof(collectorBlock);
  }
  else {
    node.next = GC.alloc;
    node.next->prev = block;
    GC.alloc = block;
    GC.blocks_alloc++;
    GC.bytes_alloc += sizeof(block) + size;
    memcpy(block, &node, sizeof(collectorBlock));
    return block + sizeof(collectorBlock);
  }
}

uintptr_t **getRoots () {
  __READ_RSP();
  uintptr_t **A = (uintptr_t **)calloc(10, sizeof(uintptr_t *));

  uint8_t * bottom = __rsp;
  uint8_t * top = (uint8_t *)GC.stack_top;
  int i = 0;
  while (bottom < top) {
    if (search(GC.addresses, (uintptr_t *) * (uintptr_t *)bottom)) {
      A[i++] = (uintptr_t *)bottom;
    }
    bottom++;
  }

  return A;
}

int main() {
  init(&GC);
  hello *a = (hello *)gc_malloc(sizeof(hello));
  hello *d = (hello *)gc_malloc(sizeof(hello));
  test *c = (test *)gc_malloc(sizeof(test));
  hello *b = (hello *)gc_malloc(sizeof(hello));

  jmp_buf jb;
  setjmp(jb);

  uintptr_t **A = getRoots();

  __READ_RSP();
  int e = 0;
  for (int i = 0; i < 10; i++) {
    if (A[i] >= (uintptr_t *)__rsp)
      e++;
  }

  printf("E: %d\n", e);

  return 0;
}