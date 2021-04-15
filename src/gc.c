#include "gc.h"
#include "hashmap.h"

// Function Prototype
map_t *getRoots();
void mark_helper(uintptr_t *);
void gc_mark();
void gc_sweep();
void gc_compact();
void computeLocations();
void updateReferences();
void relocate();

void gc_init(int argc, char **argv) {

  GC.compact_flag = 0;
  if (argc >= 2) {
    for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-gC") == 0) {
        GC.compact_flag = 1;
      }
    }
  }

  GC.stack_top = __builtin_frame_address(1);
  GC.blocks_alloc = 0;
  GC.block_head = NULL;
  GC.block_tail = NULL;
  init_set(&(GC.addresses));
}

// A recursive function to find all valid pointers from an object and mark them in a DFS manner
void mark_helper(uintptr_t *p) {
  if (!p)
    return;

  if (IS_MARKED((uintptr_t *)p))
    return;

  MARK(p);
  // Bounds of the object
  uint8_t *left = (uint8_t *)(p);
  uint8_t *right = (uint8_t *)((uint8_t *)(p) + GETSIZE(p));

  while (left < right) {
    if (search(GC.addresses, (uintptr_t *)*(uintptr_t *)left)) { // Checking if address is valid
      mark_helper((uintptr_t *)*(uintptr_t *)left); // Recursively mark the children (if any) of the object
    }
    left++;
  }

  return;
}

// A function that is used to mark all the reachable objects
void gc_mark(map_t *m) {
  hash_node **n = get_map_iter(m);

  for (int i = 0; i < m->size; i++) { // Traverse the hash map of reachable objects
    hash_node *p = n[i];
    while (p) {
      MARK(p->key); // Mark all immidiately reachable objects
      uint8_t *left = (uint8_t *)(p->key);
      uint8_t *right = (uint8_t *)((uint8_t *)(p->key) + GETSIZE(p->key));

      // Used to mark the children (if any) of the valid object
      while (left < right) {
        if (search(GC.addresses, (uintptr_t *)*(uintptr_t *)left)) {
          mark_helper((uintptr_t *)*(uintptr_t *)left);
        }
        left++;
      }
      p = p->next;
    }
  }

  return;
}

// A function that is used to free all the unreachable objects
void gc_sweep() {
  hash_node **n = get_set_iter(&(GC.addresses));

  if (!n)
    return;

  for (int i = 0; i < get_set_size(GC.addresses); i++) { // Iterate through the set that contains addresses of all malloced objects
    hash_node *p = n[i];
    while (p) {
      if (!IS_MARKED(p->key)) { // If the object is not marked then free it 
        gc_free(p->key);
      } else {
        UNMARK(p->key); // Else unmark it for future cycles
      }
      p = p->next;
    }
  }
}

// A function which helps us calculate the forwarding pointers of each object. Used in compaction
void computeLocations() {
  collectorBlock *p = GC.block_head;
  collectorBlock *q = GC.block_head;
  while (p) {
    if (p->free == 1) {
      p->forwarding_address = (uint8_t *)q;
      q = q->next;
    }
    p = p->next;
  }
}

// This function helps us to update the pointers inside objects before relocating them
void updateReferences(map_t *m) {
  hash_node **iter = get_map_iter(m);

  collectorBlock *p = GC.block_head;

  for (int i = 0; i < m->size; i++) {
    hash_node *p = iter[i];
    while (p) {
      uintptr_t *ref = (uintptr_t *)(((uint8_t *)p->key) - CBLOCK_SIZE);
      uintptr_t *data = p->data;
      if (ref) {
        p->key = (uintptr_t *)(((collectorBlock *)ref)->forwarding_address);
        *data = (uintptr_t)(((uint8_t *)(((collectorBlock *)ref)->forwarding_address)) + CBLOCK_SIZE);
      }
      p = p->next;
    }
  }

  p = GC.block_head;

  while (p) {
    if (p->free == 1) {
      uintptr_t *start = (uintptr_t *)((uint8_t *)p + CBLOCK_SIZE);
      uintptr_t *end = (uintptr_t *)((uint8_t *)p + CBLOCK_SIZE + p->size);
      while (start < end) {
        if (search(GC.addresses, (uintptr_t *)(*start))) {
          if (((uintptr_t *)(*start)) != NULL) {
            uintptr_t *ptr = (uintptr_t *)(*start);
            *start = (uintptr_t)((uint8_t *)(((collectorBlock *)((uint8_t *)ptr - CBLOCK_SIZE))->forwarding_address) + CBLOCK_SIZE);
          }
        }
        start = (uintptr_t *)((uint8_t *)start + 1); // Increment p by 1 byte
      }
    }
    p = p->next;
  }

  return;
}

// This function is used to relocate object blocks to their respective forwarding pointers
void relocate() {
  collectorBlock *p = GC.block_head;
  int garbage = 0;

  while (p) {
    if (p->free == 1) {
      uint8_t *dest = p->forwarding_address;
      memcpy((uint8_t *)dest + CBLOCK_SIZE, (uint8_t *)p + CBLOCK_SIZE,p->size); // Move `size` number of byte from src to dest
      ((collectorBlock *)dest)->size = p->size; // Copy the size of the dest block to src block
    } else {
      p->free = 1;
      garbage++;
    }
    p = p->next;
  }

  // My own workaround for compaction of objects
  // Move all valid objects to the beginning valid addresses of the heap
  // And free all the `garbage` number of objects at the end.
  collectorBlock *q = GC.block_head;
  int live_blocks = GC.blocks_alloc - garbage;

  for (int i = 0; i < live_blocks; i++)
    q = q->next;

  while (q) {
    q->free = 0;
    q = q->next;
  }

  return;
}

// An function which is the implementation of LISP2 algorithm
// This function has 3 cycles in which it succesfully relocates thus compacting
// the heap memory. 
void gc_compact(map_t *m) {
  computeLocations();
  updateReferences(m);
  destroy_map(m);
  relocate();

  return;
}

void gc_run() {
  // Roots
  // Mark
  // Sweep
  // Compact

  map_t *m = getRoots();

  gc_mark(m);

  if (GC.compact_flag == 1)
    gc_compact(m);

  gc_sweep();

  return;
}

void *gc_malloc(int size) {
  void *block = (void *)malloc(sizeof(collectorBlock) + size);
  if (!block) { // If the memory is full run the garbage collector and try mallocing again
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
  node.forwarding_address = NULL;

  if (!block)
    return NULL;

  insert(&(GC.addresses), (uintptr_t *)(block + sizeof(collectorBlock))); // Insert address into set of valid addresses
  GC.blocks_alloc++;
  memcpy(block, &node, sizeof(collectorBlock)); // Copy contents of the node into the block

  if (GC.compact_flag == 1) { // if -gC flag is set then construct linked list of objects
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
  if (!ptr) // If ptr is NULL then return
    return;

  if (!search(GC.addresses, (uintptr_t *)ptr)) // If pointer is not valid then return
    return;

  if (GC.compact_flag == 1) { // If -gC flag is set we have to remove the pointer block from the linked list of objects
    void *new_ptr = (uint8_t *)ptr - CBLOCK_SIZE;
    if (GC.block_head == GC.block_tail)
      GC.block_head = GC.block_tail = NULL;

    collectorBlock *p = GC.block_head;
    collectorBlock *q = NULL;

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
  free((uint8_t *)ptr - sizeof(collectorBlock)); // Free the block
  remov(&(GC.addresses), (uintptr_t *)ptr); // Remove address from set of all valid addresses
  GC.blocks_alloc--;

  return;
}

// This function is used to get the roots 
// (i.e. All the pointers on the stack that point to valid addresses in the heap) 
// of each object and store them in a hash map to be used later in the marking sweeping and
// compacting phase
map_t *getRoots() {

  jmp_buf jb; // Saving contents of registers on the stack
  setjmp(jb);

  __READ_RSP(); // Get the rsp of the current stack frame
  map_t *m = (map_t *)malloc(sizeof(map_t));
  initMap(m, 16);

  uint8_t *bottom = __rsp;
  uint8_t *top = (uint8_t *)GC.stack_top;
  while (bottom < top) { // Traverse the stack searching for pointers to valid addresses
    if (search(GC.addresses, (uintptr_t *)*(uintptr_t *)bottom)) {
      add_node(m, (uintptr_t *)*(uintptr_t *)bottom, (uintptr_t *)bottom); // If valid pointer found then add it to the map
    }
    bottom++;
  }
  return m;
}

void gc_dump() {
  printf("GARBAGE COLLECTOR: \n");
  if (GC.compact_flag) { // Print linked list of object if -gC flag is set
    collectorBlock *p = GC.block_head;
    while (p) {
      printf("block address: %p, memory address: %p, mark: %d, size: %d\n", p,
             (uint8_t *)p + CBLOCK_SIZE, p->free, p->size);
      p = p->next;
    }
  } else { // Print the hash set of all valid addresses
    hash_node **n = get_set_iter(&(GC.addresses));
    if (!n)
      return;

    for (int i = 0; i < get_set_size(GC.addresses); i++) {
      hash_node *p = n[i];
      while (p) {
        uint8_t *q = (uint8_t *)(p->key) - sizeof(collectorBlock);
        printf("block address: %p, memory address: %p, mark: %d, size: %d\n", q,
               p->key, IS_MARKED(p->key), GETSIZE(p->key));
        p = p->next;
      }
    }
  }
  return;
}