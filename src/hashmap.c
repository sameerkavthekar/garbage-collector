#include "hashmap.h"

void initMap(map_t *m, int size) {

	if (size < 1)
		return;

	m->buckets = (hash_node **)malloc(size * sizeof(hash_node *));
	if (!m->buckets) return;

	for (int i = 0; i <= size; i++)
		m->buckets[i] = NULL;

	m->size = size;

	return;
}

// Jenkin's 32 bit hash function (Guarantee's a good hash)
int hash_function(uintptr_t *key) {
	long a = (long)key;
	a = (a + 0x7ed55d16) + (a << 12);
	a = (a ^ 0xc761c23c) ^ (a >> 19);
	a = (a + 0x165667b1) + (a << 5);
	a = (a + 0xd3a2646c) ^ (a << 9);
	a = (a + 0xfd7046c5) + (a << 3);
	a = (a ^ 0xb55a4f09) ^ (a >> 16);
	return (int)a;
}

int add_node(map_t *m, uintptr_t *key, uintptr_t * value) {
	hash_node *nn = (hash_node *)malloc(sizeof(hash_node));
	if (!nn) return 0;
	nn->data = value;
	nn->key = key;
	nn->next = NULL;

	int index = hash_function(key) & (m->size - 1);

	hash_node *p = m->buckets[index];
	while (p) {
		if (p->key == key) {
			p->data = value;
			return 0;
		}
		p = p->next;
	}

	hash_node *temp = m->buckets[index];
	nn->next = temp;
	m->buckets[index] = nn;

	return 1;
}

int search_node(map_t *m, uintptr_t *key) {
	int index = hash_function(key) & (m->size - 1);
	hash_node *p = m->buckets[index];

	while (p) {
		if (p->key == key)
			return 1;
		p = p->next;
	}

	return 0;
}

int remove_node(map_t *m, uintptr_t *key) {
	int index = hash_function(key) & (m->size - 1);
	hash_node *p = m->buckets[index];

	if (!p)
		return 0;

	if (p->key == key) {
		m->buckets[index] = p->next;
		free(p);
		return 1;
	}

	hash_node *q = NULL;

	while (p) {
		if (p->key == key) {
			q->next = p->next;
			free(p);
			return 1;
		}
		q = p;
		p = p->next;
	}

	if (!q) {
		if (p->key == key) {
			m->buckets[index] = NULL;
			free(p);
			return 1;
		}
	}

	return 0;
}

void print_contents(map_t h) {
	printf("HASHMAP:\n");
	for (int i = 0; i <= h.size; i++) {
		printf("%d: ", i);
		hash_node *p  = h.buckets[i];
		while (p) {
			printf("%p:%p ", (void *)p->key, (void *)p->data);
			p = p->next;
		}
		printf("\n");
	}
}

void destroy_map(map_t *m) {
	if (!m) return;

	for (int i = 0; i <= m->size; i++) {
		hash_node *p = m->buckets[i];
		hash_node *q = NULL;
		while (p) {
			q = p->next;
			free(p);
			p = q;
		}
		m->buckets[i] = NULL;
	}
	free(m->buckets);
	free(m);
	m = NULL;
	return;
}

hash_node **get_map_iter(map_t *m) {
	if(!m)
		return NULL;

	return m->buckets;
}