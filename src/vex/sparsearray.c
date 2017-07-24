#include <string.h>
#include <assert.h>
#include "debug.h"
#include "sparsearray.h"

bool sparsearray_create(sparsearray *s, size_t key_capacity, size_t value_capacity) {
    if (!buffer_create(&s->keys, key_capacity)) return false;

    if (!buffer_create(&s->values, value_capacity)) {
        buffer_destroy(&s->keys);
        return false;
    }

    return true;
}

void sparsearray_destroy(sparsearray *s) {
    buffer_destroy(&s->keys);
    buffer_destroy(&s->values);
}

void sparsearray_clear(sparsearray *s) {
    buffer_clear(&s->keys);
    buffer_clear(&s->values);
}

bool sparsearray_trim(sparsearray *s) {
    return buffer_trim(&s->keys) && buffer_trim(&s->values);
}

size_t sparsearray_count(sparsearray *s) {
    return buffer_size(&s->keys) / sizeof(uint64_t);
}

size_t sparsearray_key_search(sparsearray *s, uint64_t key) {
    // Binary search for closest key index.
    size_t left = 0;
    size_t right = sparsearray_count(s);

    while (left < right) {
        size_t index = (left + right) / 2;
        uint64_t *skey = buffer_get(&s->keys, index * sizeof(uint64_t));

        if (key < *skey) {
            right = index;
        } else if (key > *skey) {
            left = index + 1;
        } else {
            left = index;
            break;
        }
    }

    return left;
}

uint64_t sparsearray_key(sparsearray *s, size_t index) {
    uint64_t *key = buffer_get(&s->keys, index * sizeof(uint64_t));
    return *key;
}

void * sparsearray_value(sparsearray *s, size_t index, size_t value_size) {
    return buffer_get(&s->values, index * value_size);
}

void * sparsearray_put(sparsearray *s, uint64_t key, size_t value_size) {
    size_t keys_size = buffer_size(&s->keys);

    if (keys_size > 0) {
        size_t index = sparsearray_key_search(s, key);
        size_t offset = index * sizeof(uint64_t);

        if (offset < keys_size) {
            uint64_t *skey = buffer_get(&s->keys, offset);

            if (*skey == key) {
                return buffer_get(&s->values, index * value_size);
            } else {
                // Insert key at offset.
                uint64_t *new_key = buffer_add(&s->keys, offset, sizeof(uint64_t));

                if (new_key == NULL) return NULL;

                *new_key = key;

                return buffer_add(&s->values, index * value_size, value_size);
            }
        } else {
            // Key is past end. Add the new key at end.
            uint64_t *new_key = buffer_push(&s->keys, sizeof(uint64_t));

            if (new_key == NULL) return NULL;

            *new_key = key;

            return buffer_push(&s->values, value_size);
        }
    } else {
        // Sparse array is empty. Add the new key.
        uint64_t *new_key = buffer_push(&s->keys, sizeof(uint64_t));

        if (new_key == NULL) return NULL;

        *new_key = key;

        return buffer_push(&s->values, value_size);
    }
}

bool sparsearray_remove(sparsearray *s, uint64_t key, size_t value_size) {
    size_t index = sparsearray_key_search(s, key);
    size_t offset = index * sizeof(uint64_t);

    if (offset >= buffer_size(&s->keys)) return false;

    uint64_t *skey = buffer_get(&s->keys, offset);

    if (*skey != key) return false;

    return buffer_remove(&s->keys, offset, sizeof(uint64_t))
        && buffer_remove(&s->values, index * value_size, value_size);
}

void * sparsearray_get(sparsearray *s, uint64_t key, size_t value_size) {
    size_t index = sparsearray_key_search(s, key);
    size_t offset = index * sizeof(uint64_t);

    if (offset >= buffer_size(&s->keys)) return NULL;

    uint64_t *skey = buffer_get(&s->keys, offset);

    if (*skey != key) return NULL;

    return buffer_get(&s->values, index * value_size);
}
