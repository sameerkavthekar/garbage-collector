#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct hash_node {
	void *data;
	void *key;
	struct hash_node *next;
} hash_node;

typedef struct hash_map {
	hash_node **buckets;
	int size;
} hash_map;

void initMap(hash_map *m, int sizek);
int add_node(hash_map *m, void *key, void *value);
int remove_node(hash_map *m, void *key);
int search_node(hash_map *m, void *key);