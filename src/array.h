// Dynamic array of 8-bit elements with length and capacity stored directly with the data.
#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    size_t capacity;
    size_t size;
    uint8_t data[];
} array;

array * array_create(size_t capacity);

void array_free(array *a);

size_t array_size(array *a);

void * array_get(array *a, size_t offset);

array * array_trim(array *a);

array * array_accomodate(array *a, size_t new_size);

array * array_insert(array *a, size_t offset, size_t bytes_size, uint8_t *bytes);

void array_delete(array *a, size_t offset, size_t bytes_size);

// Optimized array_insert for when offset equals array size
array * array_push(array *a, size_t bytes_size, uint8_t *bytes);

// Optimized array_delete for when offset equals array size - bytes_size
void array_pop(array *a, size_t bytes_size);
