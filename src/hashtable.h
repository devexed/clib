/* Sparse array, mapping 64-bit integers to arbitrary data. */
#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "buffer.h"
#include "sparsearray.h"

typedef sparsearray hashtable;

typedef struct {
    hashtable *table;
    size_t key_index;
    size_t entry_offset;
    buffer *entries;
} hashtable_iterator;

bool hashtable_create(hashtable *, size_t, size_t, size_t);

void hashtable_destroy(hashtable *);

size_t hashtable_bucket_count(hashtable *);

bool hashtable_put(hashtable *, uint64_t (*)(void *), bool (*)(void *, void *), void *, size_t, void *, size_t);

bool hashtable_remove(hashtable *, uint64_t (*)(void *), bool (*)(void *, void *), void *, size_t, size_t);

// Returns pointer to memory with key + value data in sequence.
void * hashtable_get_entry(hashtable *, uint64_t (*)(void *), bool (*)(void *, void *), void *, size_t, size_t);

void * hashtable_get_key(hashtable *, uint64_t (*)(void *), bool (*)(void *, void *), void *, size_t, size_t);

void * hashtable_get(hashtable *, uint64_t (*)(void *), bool (*)(void *, void *), void *, size_t, size_t);

hashtable_iterator hashtable_iterate(hashtable *);

bool hashtable_iterate_next(hashtable_iterator *, size_t, size_t);

void * hashtable_iterate_key(hashtable_iterator *);

void * hashtable_iterate_value(hashtable_iterator *, size_t);

#endif