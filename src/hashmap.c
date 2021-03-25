#include "hashmap.h"

void initMap(hash_map *m, int size) {
	m->buckets = (hash_node **)malloc(sizeof(hash_node *) * size);
	if (!m->buckets) return;

	for (int i = 0; i <= size; i++)
		m->buckets[i] = NULL;

	m->size = size;
	return;
}

// Jenkin's 32 bit hash function
int hash_function(void *key) {
	unsigned int a = (long)key;
	a = (a + 0x7ed55d16) + (a << 12);
	a = (a ^ 0xc761c23c) ^ (a >> 19);
	a = (a + 0x165667b1) + (a << 5);
	a = (a + 0xd3a2646c) ^ (a << 9);
	a = (a + 0xfd7046c5) + (a << 3);
	a = (a ^ 0xb55a4f09) ^ (a >> 16);
	return (int)a;
}

int add_node(hash_map *m, void *key, void *value) {
	hash_node *nn = (hash_node *)malloc(sizeof(hash_node));
	if(!nn) return;
	nn->data = value;
	nn->key = key;
	nn->next = NULL;

	int index = hash_function(key) % m->size;

	hash_node *temp = m->buckets[index];
	nn->next = temp;
	m->buckets[index] = nn;

	return 1;
}

int main() {
	void *x = (void *)0x3264527;
	int a = hash_function(x);
	printf("%d\n", a);
	return 0;
}