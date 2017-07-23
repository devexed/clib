#include <string.h>
#include <assert.h>
#include "debug.h"
#include "hashtable.h"

bool hashtable_create(hashtable *h, size_t hash_capacity, size_t key_capacity, size_t value_capacity) {
    return sparsearray_create(h, hash_capacity, key_capacity + value_capacity);
}

static inline buffer * hashtable_buffer_get(hashtable *h, size_t index, size_t value_size) {
    // Getter for buckets that asserts the bucket exists and is non-empty.
    buffer *b = sparsearray_value(h, index, value_size);
    assert(b != NULL && buffer_size(b) > 0);

    return b;
}

void hashtable_destroy(hashtable *h) {
    // Destroy all key buffers.
    for (size_t i = 0, l = sparsearray_count(h); i < l; i++)
        buffer_destroy(hashtable_buffer_get(h, i, sizeof(buffer)));

    sparsearray_destroy(h);
}

size_t hashtable_bucket_count(hashtable *h) {
    return sparsearray_count(h);
}

bool hashtable_put(hashtable *h,
                   uint64_t (*hash)(void *),
                   bool (*equals)(void *, void *),
                   void *key,
                   size_t key_size,
                   void *value,
                   size_t value_size) {
    assert(key != NULL);
    assert(value != NULL);
    uint64_t key_hash = hash(key);
    size_t entry_size = key_size + value_size;
    buffer *entries = sparsearray_get(h, key_hash, sizeof(buffer));

    if (entries == NULL) {
        entries = sparsearray_put(h, key_hash, sizeof(buffer));
        buffer_create(entries, entry_size);
    }

    for (size_t offset = 0, l = buffer_size(entries);
         offset < l;
         offset += entry_size) {
        uint8_t *bucket_entry_key = buffer_get(entries, offset);
        uint8_t *bucket_entry_value = bucket_entry_key + key_size;

        if (equals(key, bucket_entry_key)) {
            uint8_t *entry_value = bucket_entry_key + key_size;
            memmove(entry_value, value, value_size);
            return true;
        }
    }

    uint8_t *entry_key = buffer_push(entries, entry_size);
    if (entry_key == NULL) return false;

    uint8_t *entry_value = entry_key + key_size;
    memmove(entry_key, key, key_size);
    memmove(entry_value, value, value_size);

    return true;
}

bool hashtable_remove(hashtable *h,
                      uint64_t (*hash)(void *),
                      bool (*equals)(void *, void *),
                      void *key,
                      size_t key_size,
                      size_t value_size) {
    assert(key != NULL);
    uint64_t key_hash = hash(key);
    size_t entry_size = key_size + value_size;
    buffer *entries = sparsearray_get(h, key_hash, sizeof(buffer));

    if (entries == NULL) return true;

    for (size_t offset = 0, l = buffer_size(entries);
         offset < l;
         offset += entry_size) {
        uint8_t *bucket_entry_key = buffer_get(entries, offset);

        if (equals(key, bucket_entry_key)) {
            if (buffer_remove(entries, offset, entry_size)) {
                // Buffer is non-empty. Break to return success. 
                if (buffer_size(entries) > 0) break;

                // Destroy and remove buffer. Break to return success.
                buffer_destroy(entries);

                if (sparsearray_remove(h, key_hash, sizeof(buffer))) break;
            }

            // Failed to remove buffer entry or remove the entire buffer.
            return false;
        }
    }

    return true;
}

void * hashtable_get_entry(hashtable *h,
                       uint64_t (*hash)(void *),
                       bool (*equals)(void *, void *),
                       void *key,
                       size_t key_size,
                       size_t value_size) {
    assert(key != NULL);
    uint64_t key_hash = hash(key);
    buffer *entries = sparsearray_get(h, key_hash, sizeof(buffer));

    if (entries == NULL) return NULL;

    size_t entry_size = key_size + value_size;

    for (size_t offset = 0, l = buffer_size(entries);
         offset < l;
         offset += entry_size) {
        uint8_t *bucket_entry = buffer_get(entries, offset);

        if (equals(key, bucket_entry)) return bucket_entry;
    }

    return NULL;
}

void * hashtable_get(hashtable *h,
                     uint64_t (*hash)(void *),
                     bool (*equals)(void *, void *),
                     void *key,
                     size_t key_size,
                     size_t value_size) {
    uint8_t *entry = hashtable_get_entry(h, hash, equals, key, key_size, value_size);

    if (entry == NULL) return NULL;

    return entry + key_size;
}

hashtable_iterator hashtable_iterate(hashtable *h) {
    hashtable_iterator it;
    it.table = h;
    it.key_index = 0;
    it.entry_offset = 0;
    it.entries = NULL;

    return it;
}

bool hashtable_iterate_next(hashtable_iterator *it, size_t key_size, size_t value_size) {
    size_t key_count = sparsearray_count(it->table);

    // Stop if no more buckets to iterate over.
    if (it->key_index >= key_count) return false;

    if (it->entries == NULL) {
        // Entries are null, initial iteration.
        it->entry_offset = 0;
        it->entries = hashtable_buffer_get(it->table, 0, sizeof(buffer));
    } else {
        // Iterate in current entry bucket.
        it->entry_offset += key_size + value_size;
    }

    // Check if increment put us at end of buffer.
    if (it->entry_offset >= buffer_size(it->entries)) {
        it->key_index++;

        // Check if key increment put us at end of sparse array keys.
        // If so iteration has finished.
        if (it->key_index >= key_count) return false;

        // Start iterating at next entry bucket.
        it->entry_offset = 0;
        it->entries = hashtable_buffer_get(it->table, it->key_index, sizeof(buffer));
    }

    return true;
}

void * hashtable_iterate_key(hashtable_iterator *it) {
    assert(it->entries != NULL);
    
    return buffer_get(it->entries, it->entry_offset);
}

void * hashtable_iterate_value(hashtable_iterator *it, size_t key_size) {
    assert(it->entries != NULL);
    
    return (uint8_t *) buffer_get(it->entries, it->entry_offset) + key_size;
}
