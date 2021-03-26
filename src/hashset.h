#ifndef HASH_SET_GC_H_
#define HASH_SET_GC_H_

#include "hashmap.h"

#define PRESENT 0

typedef struct {
	hash_map m;
} set_t;

void init_set(set_t *s);
int insert(set_t *s, uintptr_t *key);
int remov(set_t *s, uintptr_t *key);
int search(set_t s, uintptr_t *key);
void destroy(set_t *s);

#endif