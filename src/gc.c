#include "gc.h"

map_t *getRoots();

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

// void gc_mark(map_t *A) {
//   int e = 0;
//   for (int i = 0; i < 10; i++) {
//     uint8_t * left = A[i] ? (uint8_t *) * (A[i]) : NULL;
//     printf("%p\n", left);
//     // uint8_t * right = (uint8_t *)(*(A[i]) + GETSIZE(*(A[i])));
//     //   while (left < right) {
//     //     if (search(GC.addresses, (uintptr_t *) * (uintptr_t *)left)) {
//     //       printf("%p\n", (void *) * A[i]);
//     //       e++;
//     //     }
//     //     left++;
//     //   }
//   }
//   printf("%d\n", e);
// }

void gc_run() {
  // Roots
  // Mark
  // Sweep
  // Compact

  map_t *m = getRoots();

  // gc_mark(m);


}

void *gc_malloc(int size) {
  void *block = (void *)malloc(sizeof(collectorBlock) + size);
  if (!block) {
    gc_run();
    void *block = (void *)malloc(sizeof(collectorBlock) + size);
    if (!block) {
      printf("Out of Memory\n");
      exit(0);
    }
  }

  collectorBlock node;
  node.next = NULL;
  node.prev = NULL;
  node.size = size;
  node.free = '0';

  memcpy(block, &node, sizeof(collectorBlock));

  printf("BLOCK: %p\n", block + sizeof(collectorBlock));

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

map_t *getRoots () {

  jmp_buf jb; // Saving contents of registers on the stack
  setjmp(jb);

  __READ_RSP();
  map_t *m = (map_t *)malloc(sizeof(map_t));
  initMap(m, 16);

  uint8_t * bottom = __rsp;
  uint8_t * top = (uint8_t *)GC.stack_top;
  while (bottom < top) {
    if (search(GC.addresses, (uintptr_t *) * (uintptr_t *)bottom)) {
      add_node(m, (uintptr_t *) * (uintptr_t *)bottom, (uintptr_t *)bottom);
    }
    bottom++;
  }
  print_contents(*m);
  return m;
}

int main() {
  init(&GC);

  hello *a = (hello *)gc_malloc(sizeof(hello));
  hello *e = (hello *)gc_malloc(sizeof(hello));
  a->next = e;
  e = NULL;
  hello *d = (hello *)gc_malloc(sizeof(hello));
  test *c = (test *)gc_malloc(sizeof(test));
  hello *b = (hello *)gc_malloc(sizeof(hello));

  gc_run();

  return 0;
}