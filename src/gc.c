#include "gc.h"

void init(garbageCollector *gc) {
  gc->frees = 0;
  gc->mallocs = 0;
  gc->stack_bottom = __builtin_frame_address(0);
  gc->heap_top = sbrk(0);
  gc->alloc = NULL;
  gc->free = NULL;
  gc->bytes_alloc = 0;
  gc->blocks_alloc = 0;
}

void *gc_malloc(garbageCollector *gc, size_t size) {
  void *block = (void *)malloc(sizeof(collectorBlock) + size);
  collectorBlock node;
  node.next = NULL;
  node.prev = NULL;
  node.size = size;
  node.free = 0;

  if (!block)
    return NULL;

  if (!gc->alloc) {
    gc->alloc = block;
    gc->blocks_alloc++;
    gc->bytes_alloc = sizeof(block) + size;
    return block + size;
  }

  return NULL;
}