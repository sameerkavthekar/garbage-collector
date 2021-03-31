#include "gc.h"

map_t *getRoots();
void gc_dump();

typedef struct hello {
  int a;
  struct hello *next;
} hello;

typedef struct test {
  char a, b, c, d;
} test;

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

void mark_helper(uintptr_t *p) {
  if (!p)
    return;

  if (IS_MARKED((uintptr_t *)p))
    return;

  MARK(p);
  uint8_t *left = (uint8_t *)(p);
  uint8_t *right = (uint8_t *)((uint8_t *)(p) + GETSIZE(p));

  while (left < right) {
    if (search(GC.addresses, (uintptr_t *) * (uintptr_t *)left)) {
      mark_helper((uintptr_t *) * (uintptr_t *)left);
    }
    left++;
  }

  return;
}


void gc_mark() {
  map_t *m = getRoots();
  hash_node **n = get_map_iter(m);

  for (int i = 0; i < m->size; i++) {
    hash_node *p = n[i];
    while (p) {
      MARK(p->key);
      uint8_t *left = (uint8_t *)(p->key);
      uint8_t *right = (uint8_t *)((uint8_t *)(p->key) + GETSIZE(p->key));

      while (left < right) {
        if (search(GC.addresses, (uintptr_t *) * (uintptr_t *)left)) {
          mark_helper((uintptr_t *) * (uintptr_t *)left);
        }
        left++;
      }
      p = p->next;
    }
  }

  destroy_map(m);
  return;
}

void gc_run() {
  // Roots
  // Mark
  // Sweep
  // Compact

  gc_mark();
  // gc_sweep();
  // gc_compact();
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
  node.size = size;
  node.free = 0;

  memcpy(block, &node, sizeof(collectorBlock));

  // printf("BLOCK: %p\n", block + sizeof(collectorBlock));

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
  return m;
}

void gc_dump() {
  collectorBlock *p = GC.alloc;

  if (!p)
    return;

  printf("GARBAGE COLLECTOR: \n");
  while (p) {
    printf("block address: %p, memory address: %p, mark: %d, size: %d\n", p, (uint8_t *)p + sizeof(collectorBlock), p->free, p->size);
    p = p->next;
  }
}

int main() {
  gc_init();

  hello *a = (hello *)gc_malloc(sizeof(hello));
  hello *e = (hello *)gc_malloc(sizeof(hello));
  hello *f = (hello *)gc_malloc(sizeof(hello));
  a->next = e;
  e->next = f;
  f = NULL;
  e = NULL;
  hello *d = (hello *)gc_malloc(sizeof(hello));
  d->next = NULL;
  test *c = (test *)gc_malloc(sizeof(test));
  hello *b = (hello *)gc_malloc(sizeof(hello));
  b->next = NULL;

  gc_run();

  gc_dump();

  return 0;
}