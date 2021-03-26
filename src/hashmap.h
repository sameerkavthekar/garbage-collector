#ifndef HASH_MAP_GC_H_
#define HASH_MAP_GC_H_

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

#define PRESENT 0

typedef struct hash_node {
	uintptr_t * data;
	uintptr_t *key;
	struct hash_node *next;
	int index;
} hash_node;

typedef struct hash_map {
	hash_node **buckets;
	int size;
} hash_map;

typedef hash_map map_t;

void initMap(map_t *m, int size);
int add_node(map_t *m, uintptr_t *key, uintptr_t * value);
int remove_node(map_t *m, uintptr_t *key);
int search_node(map_t *m, uintptr_t *key);
void destroy_map(map_t *m);
void print_contents(map_t h);

#endif