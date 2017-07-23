#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "array.h"
#include "debug.h"

array * array_create(size_t capacity) {
    array *s = malloc(sizeof(array) + capacity);

    if (s == NULL) return NULL;

    s->capacity = capacity;
    s->size = 0;

    return s;
}

size_t array_size(array *s) {
    return s->size;
}

void * array_get(array *s, size_t offset) {
    return s->data + offset;
}

void array_free(array *s) {
    free(s);
}

array * array_trim(array *s) {
    // Return NULL early to simplify chaining array operations.
    if (s == NULL) return NULL;

    if (s->capacity != s->size) {
        array *new_s = realloc(s, sizeof(array) + s->size);

        if (new_s != NULL) {
            s = new_s;
            s->capacity = s->size;
        }
    }

    return s;
}

array * array_accomodate(array *s, size_t new_size) {
    // Return NULL early to simplify chaining array operations.
    if (s == NULL) return NULL;

    if (new_size > s->capacity) {
        // New capacity, increased by 1.5. Ensure new capacity actually
        // accomodates the new size by otherwise simply using the new size.
        size_t new_capacity = max(new_size, (s->capacity * 3) / 2);

        if (new_capacity < new_size) new_capacity = new_size;

        // Reallocate array and update capacity if successful.
        s = realloc(s, sizeof(array) + new_capacity);

        if (s != NULL) s->capacity = new_capacity;
    }

    return s;
}

array * array_insert(array *s, size_t offset, size_t bytes_size, uint8_t *bytes) {
    // Return NULL early to simplify chaining array operations.
    if (s == NULL) return NULL;

    assert(offset <= s->size);

    size_t new_size = s->size + bytes_size;
    s = array_accomodate(s, new_size);

    if (s != NULL) {
        // Push data after offset forward.
        memmove(s->data + offset + bytes_size, s->data + offset, s->size - offset);

        // Copy new data into array.
        // Unable to use memcpy as the bytes argument might point into the
        // data array.
        memmove(s->data + offset, bytes, bytes_size);
        s->size = new_size;
    }

    return s;
}

void array_delete(array *s, size_t offset, size_t bytes_size) {
    // Return early to simplify chaining array operations.
    if (s == NULL) return;

    assert(offset <= s->size);
    assert(bytes_size <= s->size);
    assert(offset + bytes_size <= s->size);

    // Push data after offset + size backward.
    memmove(s->data + offset, s->data + offset + bytes_size, bytes_size);
    s->size -= bytes_size;
}

array * array_push(array *s, size_t bytes_size, uint8_t *bytes) {
    // Return NULL early to simplify chaining array operations.
    if (s == NULL) return NULL;

    size_t new_size = s->size + bytes_size;
    s = array_accomodate(s, new_size);

    if (s != NULL) {
        // Copy new data into array.
        // Can be memcpy rather than memmove as the destination copied
        // into is previously unused and cannot overlap with the bytes
        // argument.
        memcpy(s->data + s->size, bytes, bytes_size);
        s->size = new_size;
    }

    return s;
}

void array_pop(array *s, size_t bytes_size) {
    assert(bytes_size <= s->size);

    s->size -= bytes_size;
}