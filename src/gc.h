#ifndef GARBAGE_COLLECTOR_GC_H_
#define GARBAGE_COLLECTOR_GC_H_

#include <stddef.h> // For size_t
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sbrk
#include <string.h>
#include <setjmp.h>
#include <stdint.h>

uint8_t *__rsp;

#define __READ_RSP() __asm__ volatile("movq %%rsp, %0" : "=r"(__rsp))
#define GETNEXT(a) (void *)(((collectorBlock *)((unsigned char *)a - sizeof(collectorBlock))) -> next)
#define GETPREV(a) (void *)(((collectorBlock *)((unsigned char *)a - sizeof(collectorBlock))) -> prev)
#define GETSIZE(a) (((collectorBlock *)((unsigned char *)a - sizeof(collectorBlock))) -> size)

/*
    Make sure you can collate the global roots. These are the local and global
   variables that contain references into the heap.

   POSSIBLE: For local variables, push
   them on to a shadow stack for the duration of their scope.

    Make sure you can traverse the heap, e.g. every value in the heap is an
   object that implements a Visit method.

    Keep the list of all allocated values.

    Allocate by calling malloc and inserting the pointer into the listS of all
   allocated values.

    When the total size of all allocated values exceeds a quota, kick off the
   mark and then sweep phases. This recursively traverses the heap accumulating
   the set of all reachable values.

    The list difference of the allocated values minus the reachable values is
   the list of unreachable values. Iterate over them calling free and removing
   them from the list of allocated values.

    POSSIBLE: Set the quota to twice the total size of all allocated values.
*/

#define CBLOCK_SIZE sizeof(collectorBlock)

// Every node in the free/non-free list
typedef struct collectorBlock {
  struct collectorBlock *next, *prev;
  int size;
  char free;
} collectorBlock;

// A doubly linked list
typedef struct dlist {
  collectorBlock *head, *tail;
} dlist;

// The actual Garbage Collector Object
typedef struct garbageCollector {
  void *stack_top, *stack_bottom, *heap_top, *heap_bottom;
  collectorBlock *free, *alloc;
  int mallocs, frees, bytes_alloc, blocks_alloc;
} garbageCollector;

garbageCollector GC; // Global GC object

void init(garbageCollector *gc);
void *gc_malloc(garbageCollector *gc, int size);
void gc_calloc(garbageCollector *gc, int num_blocks, int size);
void gc_realloc(garbageCollector *gc, void *ptr, int size);
void gc_free(garbageCollector *gc);
void gc_mark(garbageCollector *gc);
void gc_sweep(garbageCollector *gc);
void gc_defragment(garbageCollector *gc);

#endif

// ***** keep in mind after accessing next and prev add subtract 24 to get the actual structure