#include "gc.h"
#include <stdint.h>

map_t *getRoots();

void gc_init(int argc, char **argv) {

  GC.compact_flag = 0;
  if (argc >= 2) {
    for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-gC") == 0) {
        GC.compact_flag = 1;
      }
    }
  }

  GC.frees = 0;
  GC.mallocs = 0;
  GC.stack_top = __builtin_frame_address(1);
  GC.heap_top = sbrk(0);
  GC.bytes_alloc = 0;
  GC.blocks_alloc = 0;
  GC.block_head = NULL;
  GC.block_tail = NULL;
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
    if (search(GC.addresses, (uintptr_t *)*(uintptr_t *)left)) {
      mark_helper((uintptr_t *)*(uintptr_t *)left);
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
        if (search(GC.addresses, (uintptr_t *)*(uintptr_t *)left)) {
          mark_helper((uintptr_t *)*(uintptr_t *)left);
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
      } else {
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

  gc_sweep();

  collectorBlock *p = GC.block_head;

  if (GC.compact_flag == 1) {
    // gc_compact();
  }
  return;
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
  node.next = NULL;

  memcpy(block, &node, sizeof(collectorBlock));

  if (!block)
    return NULL;

  insert(&(GC.addresses), (uintptr_t *)(block + sizeof(collectorBlock)));
  GC.blocks_alloc++;
  GC.bytes_alloc += sizeof(collectorBlock) + size;
  memcpy(block, &node, sizeof(collectorBlock));

  if (GC.compact_flag == 1) {
    if (GC.block_head == NULL) {
      GC.block_head = GC.block_tail = (collectorBlock *)block;
    } else {
      collectorBlock *p = GC.block_tail;
      p->next = (collectorBlock *)block;

      GC.block_tail = (collectorBlock *)block;
    }
  }

  return block + sizeof(collectorBlock);
}

void gc_free(void *ptr) {
  if (!ptr)
    return;

  if (!search(GC.addresses, (uintptr_t *)ptr))
    return;

  if (GC.compact_flag == 1) {
    void *new_ptr = (uint8_t *)ptr - CBLOCK_SIZE;
    collectorBlock *p = GC.block_head;
    collectorBlock *q = NULL;
    if (GC.block_head == GC.block_tail)
      GC.block_head = GC.block_tail = NULL;

    while (p) {
      if (p == new_ptr) {
        if (p->next == NULL) {
          q->next = NULL;
          GC.block_tail = q;
          break;
        } else if (q == NULL) {
          GC.block_head = p->next;
          break;
        }

        q->next = p->next;
        break;
      }
      q = p;
      p = p->next;
    }
  }

  int size = GETSIZE(ptr);
  free((uint8_t *)ptr - sizeof(collectorBlock));
  remov(&(GC.addresses), (uintptr_t *)ptr);
  GC.blocks_alloc--;
  GC.bytes_alloc -= sizeof(collectorBlock) + size;

  return;
}

map_t *getRoots() {

  jmp_buf jb; // Saving contents of registers on the stack
  setjmp(jb);

  __READ_RSP();
  map_t *m = (map_t *)malloc(sizeof(map_t));
  initMap(m, 16);

  uint8_t *bottom = __rsp;
  uint8_t *top = (uint8_t *)GC.stack_top;
  while (bottom < top) {
    if (search(GC.addresses, (uintptr_t *)*(uintptr_t *)bottom)) {
      add_node(m, (uintptr_t *)*(uintptr_t *)bottom, (uintptr_t *)bottom);
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
      printf("block address: %p, memory address: %p, mark: %d, size: %d\n", q,
             p->key, IS_MARKED(p->key), GETSIZE(p->key));
      p = p->next;
    }
  }

  return;
}