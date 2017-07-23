/* Simple dynamic buffer. */
#ifndef BUFFER_H
#define BUFFER_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    size_t size;
    size_t capacity;
    uint8_t *data;
} buffer;

bool buffer_create(buffer *, size_t);

void buffer_destroy(buffer *);

void buffer_clear(buffer *);

bool buffer_trim(buffer *);

size_t buffer_size(buffer *);

void * buffer_push(buffer *, size_t);

void buffer_pop(buffer *, size_t);

void * buffer_add(buffer *, size_t, size_t);

void * buffer_resize(buffer *, size_t, size_t, size_t);

bool buffer_remove(buffer *, size_t, size_t);

void * buffer_get(buffer *, size_t); 

bool buffer_push_buffer(buffer *, buffer *); 

#endif