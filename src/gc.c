#include "gc.h"

map_t *getRoots();

void gc_init() {
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

void gc_sweep() {
  hash_node **n = get_set_iter(&(GC.addresses));

  if (!n)
    return;

  for (int i = 0; i < get_set_size(GC.addresses); i++) {
    hash_node *p = n[i];
    while (p) {
      if (!IS_MARKED(p->key)) {
        gc_free(p->key);
      }
      else {
        UNMARK(p->key);
      }
      p = p->next;
    }
  }
}

void gc_run() {
  // Roots
  // Mark
  // Sweep
  // Compact

  gc_mark();
  gc_dump();
  gc_sweep();
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
  node.size = size;
  node.free = 0;

  memcpy(block, &node, sizeof(collectorBlock));

  if (!block)
    return NULL;

  insert(&(GC.addresses), (uintptr_t *)(block + sizeof(collectorBlock)));
  GC.blocks_alloc++;
  GC.bytes_alloc += sizeof(collectorBlock) + size;
  memcpy(block, &node, sizeof(collectorBlock));
  return block + sizeof(collectorBlock);
}

void gc_free(void *ptr) {
  if (!ptr)
    return;

  if (!search(GC.addresses, (uintptr_t *)ptr))
    return;

  int size = GETSIZE(ptr);
  free((uint8_t *)ptr - sizeof(collectorBlock));
  remov(&(GC.addresses), (uintptr_t *)ptr);
  GC.blocks_alloc--;
  GC.bytes_alloc -= sizeof(collectorBlock) + size;
  return;
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
  hash_node **n = get_set_iter(&(GC.addresses));

  if (!n)
    return;

  printf("GARBAGE COLLECTOR: \n");
  for (int i = 0; i < get_set_size(GC.addresses); i++) {
    hash_node *p = n[i];
    while (p) {
      uint8_t *q = (uint8_t *)(p->key) - sizeof(collectorBlock);
      printf("block address: %p, memory address: %p, mark: %d, size: %d\n", q, p->key, IS_MARKED(p->key), GETSIZE(p->key));
      p = p->next;
    }
  }

  return;
}