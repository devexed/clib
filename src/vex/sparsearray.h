/* Sparse array, mapping 64-bit integers to arbitrary data. */
#ifndef SPARSEARRAY_H
#define SPARSEARRAY_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "buffer.h"

typedef struct {
    buffer keys;
    buffer values;
} sparsearray;

bool sparsearray_create(sparsearray *, size_t, size_t);

void sparsearray_destroy(sparsearray *);

void sparsearray_clear(sparsearray *);

bool sparsearray_trim(sparsearray *);

size_t sparsearray_count(sparsearray *);

uint64_t sparsearray_key(sparsearray *, size_t);

void * sparsearray_value(sparsearray *, size_t, size_t);

void * sparsearray_put(sparsearray *, uint64_t, size_t);

bool sparsearray_remove(sparsearray *, uint64_t, size_t);

void * sparsearray_get(sparsearray *, uint64_t, size_t);

#endif