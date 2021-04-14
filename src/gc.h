#ifndef GARBAGE_COLLECTOR_GC_H_
#define GARBAGE_COLLECTOR_GC_H_

#include <setjmp.h> // For jmp_buf
#include <stdint.h> // For uint8_t and uintptr_t
#include <stdio.h>  // I/O
#include <stdlib.h> // For dynamic memory allocation
#include <string.h> // For memcpy
#include <unistd.h> // For sbrk

#include "hashset.h"

static uint8_t *__rsp;

#define __READ_RSP() __asm__ volatile("movq %%rsp, %0" : "=r"(__rsp))
#define GETSIZE(a)                                                             \
  (((collectorBlock *)((uint8_t *)a - sizeof(collectorBlock)))->size)
#define MARK(a)                                                                \
  ((collectorBlock *)(((uint8_t *)a) - sizeof(collectorBlock)))->free = 1
#define UNMARK(a)                                                              \
  ((collectorBlock *)(((uint8_t *)a) - sizeof(collectorBlock)))->free = 0
#define IS_MARKED(a)                                                           \
  ((((collectorBlock *)(((uint8_t *)a) - sizeof(collectorBlock)))->free) == 1)
#define CBLOCK_SIZE sizeof(collectorBlock)

// Header to each block
typedef struct collectorBlock {
  struct collectorBlock *next;
  uint8_t *forwarding_address;
  int size;
  char free;
} collectorBlock;

// The actual Garbage Collector Object
static struct gc {
  void *stack_top;
  int blocks_alloc;
  set_t addresses;
  collectorBlock *block_head, *block_tail;
  char compact_flag;
} GC;

// Function gc_init()
// Params: argc -> argument count, argv -> argument vector
// Returns: void
// Purpose: Initialises the garbage collector object by setting many required
// values
void gc_init(int argc, char **argv);

// Function gc_malloc()
// Params: Size of the block
// Returns: A pointer to the malloced memory
// Purpose: Helps in dynamic memory allocation and registers addresses of all
// valid objects
void *gc_malloc(int size);

// Function gc_free()
// Params: Address of block to be freed
// Returns: void
// Purpose: Frees the block that was allocated by gc_malloc
void gc_free(void *ptr);

// Function gc_run()
// Params: None
// Returns: void
// Purpose: It is used to run the garbage collector any time in the program
// cycle
void gc_run();

// Function gc_dump()
// Params: None
// Returns: void
// Purpose: It is used to print the contents of garbage collector any time in
// the program cycle
void gc_dump();

#endif