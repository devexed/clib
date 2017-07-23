#include <string.h>
#include <assert.h>
#include "buffer.h"
#include "debug.h"

bool buffer_set_capacity(buffer *, size_t);

bool buffer_create(buffer *b, size_t capacity) {
    void *new_data = malloc(capacity);

    if (new_data == NULL) return false;

    b->data = new_data;
    b->size = 0;
    b->capacity = capacity;

    return true;
}

void buffer_destroy(buffer *b) {
    free(b->data);
}

void buffer_clear(buffer *b) {
    b->size = 0;
}

bool buffer_trim(buffer *b) {
    return buffer_set_capacity(b, b->size);
}

size_t buffer_size(buffer *b) {
    return b->size;
}

bool buffer_set_capacity(buffer *b, size_t new_capacity) {
    assert(new_capacity > 1); // Capacity must be greater than one to grow.
    uint8_t *new_data = realloc(b->data, new_capacity);

    if (new_data == NULL) return false;

    b->data = new_data;
    b->capacity = new_capacity;

    return true;
}

bool buffer_ensure_capacity(buffer *b, size_t new_size) {
    if (b->capacity < new_size) {
        size_t new_capacity = (b->capacity * 3) / 2;

        if (new_capacity < new_size) new_capacity = new_size;

        return buffer_set_capacity(b, new_capacity);
    }

    return true;
}

void * buffer_push(buffer *b, size_t data_size) {
    size_t new_size = b->size + data_size;

    if (!buffer_ensure_capacity(b, new_size)) return NULL;

    assert(b->size + data_size <= b->capacity);
    uint8_t *data = b->data + b->size;
    b->size = new_size;

    return data;
}

void buffer_pop(buffer *b, size_t data_size) {
    assert(data_size <= b->size);
    b->size = b->size - data_size;
}

void * buffer_resize(buffer *b, size_t data_offset, size_t old_data_size, size_t new_data_size) {
    assert(data_offset <= b->size);
    assert(data_offset + old_data_size <= b->size);
    size_t new_size = b->size - old_data_size + new_data_size;

    if (!buffer_ensure_capacity(b, new_size)) return NULL;

    assert(b->size + new_data_size <= b->capacity);
    uint8_t *data = b->data + data_offset;
    memmove(data + new_data_size, data + old_data_size, b->size - old_data_size - data_offset);
    b->size = new_size;

    return data;
}

void * buffer_add(buffer *b, size_t data_offset, size_t data_size) {
    return buffer_resize(b, data_offset, 0, data_size);
}

bool buffer_remove(buffer *b, size_t data_offset, size_t data_size) {
    return buffer_resize(b, data_offset, data_size, 0) != NULL;
}

void * buffer_get(buffer *b, size_t data_offset) {
    assert(data_offset < b->size);

    return b->data + data_offset;
}

bool buffer_push_buffer(buffer *target, buffer *source) {
    size_t size = buffer_size(source);
    void *target_data = buffer_push(target, size);

    if (target_data == NULL) return false;

    void *source_data = buffer_get(source, 0);
    memmove(target_data, source_data, size);

    return true;
}

