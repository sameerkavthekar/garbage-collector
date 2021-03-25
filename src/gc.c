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
  gc->stack_bottom = gc->stack_top = __builtin_frame_address(1);
  gc->heap_top = sbrk(0);
  gc->alloc = NULL;
  gc->free = NULL;
  gc->bytes_alloc = 0;
  gc->blocks_alloc = 0;
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

  return NULL;
}

uintptr_t **getRoots () {
  // THIS SHOULD ACTUALLY BE A SET AND NOT AN ARRAY
  uintptr_t **A = (uintptr_t **)malloc(sizeof(uintptr_t *));
  jmp_buf jb;
  setjmp(jb);
  __READ_RSP();
  uint8_t * bottom = __rsp;
  uint8_t * top = (uint8_t *)GC.stack_top;
  int i = 0;
  while (bottom < top) {
    // Implement an AVL tree or a SET too lookup if the address is valid
    if (a == (void *) * (uintptr_t *)bottom) {
      A[i++] = (uintptr_t *)bottom;
    }
    bottom++;
  }
  printf("I: %d", i);
  return A;
}

int main() {
  init(&GC);
  hello *a = (hello *)gc_malloc(&GC, sizeof(hello));
  hello *b = (hello *)gc_malloc(&GC, sizeof(hello));
  test *c = (test *)gc_malloc(&GC, sizeof(test));

  getRoots();

  return 0;
}