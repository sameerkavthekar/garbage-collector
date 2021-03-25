#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

typedef struct hash_node {
	char data;
	uintptr_t *key;
	struct hash_node *next;
} hash_node;

typedef struct hash_map {
	hash_node **buckets;
	int size;
} hash_map;

void initMap(hash_map *m, int size);
int add_node(hash_map *m, uintptr_t *key, char value);
int remove_node(hash_map *m, uintptr_t *key);
int search_node(hash_map *m, uintptr_t *key);
void destroy_map(hash_map *m);