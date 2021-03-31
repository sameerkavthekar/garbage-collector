#include "hashset.h"

void init_set(set_t *s) {
	initMap(&(s->m), 16);
	return;
}

int insert(set_t *s, uintptr_t *key) {
	return add_node(&(s->m), key, PRESENT);
}

int remov(set_t *s, uintptr_t *key) {
	return remove_node(&(s->m), key);
}

int search(set_t s, uintptr_t *key) {
	return search_node(&(s.m), key);
}

void destroy(set_t *s) {
	destroy_map(&(s->m));
}

hash_node **get_set_iter(set_t *s) {
	if(!s)
		return NULL;

	return s->m.buckets;
}

int get_set_size(set_t s) {
	return s.m.size;
}