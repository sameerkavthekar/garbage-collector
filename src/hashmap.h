#ifndef HASH_MAP_GC_H_
#define HASH_MAP_GC_H_

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PRESENT 0

typedef struct hash_node {
  uintptr_t *data;
  uintptr_t *key;
  struct hash_node *next;
} hash_node;

typedef struct hash_map {
  hash_node **buckets;
  int size;
} hash_map;

typedef hash_map map_t;

// Function initMap()
// Params: Size of hashmap
// Returns: void
// Purpose: Initialises hashmap with necessary fields
void initMap(map_t *m, int size);

// Function add_node()
// Params: Address of map, key and value to be inserted
// Returns: Integer indicating success of operation
// Purpose: Adds a key value pair to the map
int add_node(map_t *m, uintptr_t *key, uintptr_t *value);

// Function remove_node()
// Params: Address of map, key to be removed
// Returns: Integer indicating success of operation
// Purpose: Removes a key value pair from the map
int remove_node(map_t *m, uintptr_t *key);

// Function search_node()
// Params: Address of map, key to be searched
// Returns: Integer indicating if the key is found or not
// Purpose: Searches for given key in the map
int search_node(map_t *m, uintptr_t *key);

// Function destroy_map()
// Params: Address of map
// Returns: void
// Purpose: Destroys the map freeing all malloced objects
void destroy_map(map_t *m);

// Function print_contents()
// Params: Map object
// Returns: void
// Purpose: Prints contents of the map in `key:value` fashion
void print_contents(map_t h);

// Function get_map_iter()
// Params: Map object
// Returns: An iterator to the map allowing us to traverse the map
// Purpose: Allows us to traverse the map if necessary
hash_node **get_map_iter(map_t *m);

#endif