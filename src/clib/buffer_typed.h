// Extends buffer.h with a macro to create typed buffers for additional safety. 
#ifndef BUFFER_TYPED_H
#define BUFFER_TYPED_H
#include "buffer.h"

#define BUFFER_REGISTER_TYPE(name, type) \
    typedef struct { buffer b; } buffer_ ## name; \
    inline static bool buffer_create_ ## name(buffer_ ## name *bt, size_t initial_capacity) { \
        return buffer_create(&bt->b, initial_capacity * sizeof(type)); \
    } \
    inline static void buffer_destroy_ ## name(buffer_ ## name *bt) { \
        buffer_destroy(&bt->b); \
    } \
    inline static void buffer_clear_ ## name(buffer_ ## name *bt) { \
        buffer_clear(&bt->b); \
    } \
    inline static bool buffer_trim_ ## name(buffer_ ## name *bt) { \
        return buffer_trim(&bt->b); \
    } \
    inline static size_t buffer_size_ ## name(buffer_ ## name *bt) { \
        return buffer_size(&bt->b) / sizeof(type); \
    } \
    inline static type * buffer_push_ ## name(buffer_ ## name *bt, size_t count) { \
        return buffer_push(&bt->b, count * sizeof(type)); \
    } \
    inline static void buffer_pop_ ## name(buffer_ ## name *bt, size_t count) { \
        buffer_pop(&bt->b, count * sizeof(type)); \
    } \
    inline static type * buffer_add_ ## name(buffer_ ## name *bt, size_t offset, size_t count) { \
        return buffer_add(&bt->b, offset * sizeof(type), count * sizeof(type)); \
    } \
    inline static type * buffer_resize_ ## name(buffer_ ## name *bt, size_t offset, size_t old_count, size_t new_count) { \
        return buffer_resize(&bt->b, offset * sizeof(type), old_count * sizeof(type), new_count * sizeof(type)); \
    } \
    inline static bool buffer_remove_ ## name(buffer_ ## name *bt, size_t offset, size_t count) { \
        return buffer_remove(&bt->b, offset * sizeof(type), count * sizeof(type)); \
    } \
    inline static type * buffer_get_ ## name(buffer_ ## name *bt, size_t offset) { \
        return buffer_get(&bt->b, offset * sizeof(type)); \
    } \
    inline static bool buffer_copy_ ## name(buffer_ ## name *target, buffer_ ## name *source) { \
        return buffer_copy(&target->b, &source->b); \
    }

#endif