#ifndef HASH_SET_GC_H_
#define HASH_SET_GC_H_

#include "hashmap.h"

#define PRESENT 0

typedef struct {
	hash_map m;
} set_t;

// Function init_set()
// Params: Address of set
// Returns: void
// Purpose: Initialises hashset with appropriate fields
void init_set(set_t *s);

// Function insert()
// Params: Address of set, Key to be inserted
// Returns: Integer indicating success of operation
// Purpose: Inserts value into set
int insert(set_t *s, uintptr_t *key);

// Function remov()
// Params: Address of set, key to be removed
// Returns: Integer indicating success of operation
// Purpose: Removes particular key from set
int remov(set_t *s, uintptr_t *key);

// Function search()
// Params: Set object, key to be searched
// Returns: Integer indicating if the key is found or not
// Purpose: Searches for given key in the set
int search(set_t s, uintptr_t *key);

// Function destroy()
// Params: Address of set
// Returns: void
// Purpose: Destroys given set
void destroy(set_t *s);

// Function get_set_iter()
// Params: Set object
// Returns: An iterator to the set allowing us to traverse the set
// Purpose: Allows us to traverse the set if necessary
hash_node **get_set_iter(set_t *s);

// Function get_set_size()
// Params: Set object
// Returns: Size of object
// Purpose: Used for getting size of object
int get_set_size(set_t s);

#endif