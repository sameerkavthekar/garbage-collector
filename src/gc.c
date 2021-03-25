#include "gc.h"

typedef struct hello {
  int a;
} hello;

typedef struct test {
  char a, b, c, d;
} test;

// TODO: IMPLEMENT AVL TREE TO SEARCH FOR VALID ADDRESSES
// TODO: IMPLEMENT A SET TO HOLD ALL THE ADDRESSES INSTEAD OF ARRAY


void init(garbageCollector *gc) {
  gc->frees = 0;
  gc->mallocs = 0;
  gc->stack_top = __builtin_frame_address(1);
  gc->heap_top = sbrk(0);
  gc->alloc = NULL;
  gc->free = NULL;
  gc->bytes_alloc = 0;
  gc->blocks_alloc = 0;
  init_set(&(gc->addresses));
}

void *gc_malloc(garbageCollector *gc, int size) {
  void *block = (void *)malloc(sizeof(collectorBlock) + size);
  collectorBlock node;
  node.next = NULL;
  node.prev = NULL;
  node.size = size;
  node.free = '0';

  memcpy(block, &node, sizeof(collectorBlock));
  // printf("%d %ld\n", size, sizeof(collectorBlock));
  printf("BLOCK: %p\n", block);
  printf("BLOCK2: %p\n", block + sizeof(collectorBlock));


  if (!block)
    return NULL;

  insert(&(gc->addresses), (uintptr_t *)(block + sizeof(collectorBlock)));

  if (!gc->alloc) {
    gc->alloc = block;
    gc->blocks_alloc++;
    gc->bytes_alloc += sizeof(block) + size;
    return block + sizeof(collectorBlock);
  }
  else {
    node.next = gc->alloc;
    node.next->prev = block;
    gc->alloc = block;
    gc->blocks_alloc++;
    gc->bytes_alloc += sizeof(block) + size;
    memcpy(block, &node, sizeof(collectorBlock));
    return block + sizeof(collectorBlock);
  }
}

void getRoots () {
  jmp_buf jb;
  setjmp(jb);

  __READ_RSP();

  uint8_t * bottom = __rsp;
  uint8_t * top = (uint8_t *)GC.stack_top;

  int i = 0;
  while (bottom < top) {
    if (search(GC.addresses, (uintptr_t *)*(uintptr_t *)bottom)) {
      printf("%p\n", (void *)*(uintptr_t *)bottom);
      i++;
    }
    bottom++;
  }
  printf("I: %d\n", i);
  return;
}

int main() {
  init(&GC);
  hello *a = (hello *)gc_malloc(&GC, sizeof(hello));
  hello *b = (hello *)gc_malloc(&GC, sizeof(hello));
  test *c = (test *)gc_malloc(&GC, sizeof(test));

  getRoots();

  return 0;
}