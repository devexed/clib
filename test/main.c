#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../src/debug.h"
#include "../src/test.h"
#include "../src/utf8string.h"
#include "../src/buffer.h"
#include "../src/buffer_typed.h"
#include "../src/sparsearray.h"
#include "../src/hashtable.h"
#include "../src/hashtable_typed.h"

test buffer_test() {
    buffer b;
    bool b_init = buffer_create(&b, 10);
    expect(b_init, "Failed to create buffer");

    // Add 100 in reverse order
    for (size_t i = 0; i < 100; i++) {
        size_t *v = buffer_add(&b, 0, sizeof(size_t));
        expect(v != NULL, "Failed to insert into buffer");
        *v = i;
    }

    // Remove 50 ints from the middle
    for (size_t i = 75 - 1; i > 25; i--) {
        bool b_remove = buffer_remove(&b, i * sizeof(size_t), sizeof(size_t));
        expect(b_remove, "Failed to remove from buffer");
    }

    // Check first 25 ints
    for (size_t i = 0; i < 25; i++) {
        size_t *n = buffer_get(&b, i * sizeof(size_t));
        expect(*n == 99 - i, "Unexpected value");
    }

    // Check last 25 ints
    for (size_t i = 25; i < 25; i++) {
        size_t *n = buffer_get(&b, i * sizeof(size_t));
        expect(*n == 99 - i, "Unexpected value");
    }

    buffer_destroy(&b);
    succeed;
}

BUFFER_REGISTER_TYPE(long, long)

test buffer_typed_test() {
    buffer_long b;
    bool b_init = buffer_create_long(&b, 10);
    expect(b_init, "Failed to create buffer");

    // Add 100 chars in reverse order
    for (size_t i = 0; i < 100; i++) {
        long *v = buffer_add_long(&b, 0, 1);
        expect(v != NULL, "Failed to insert into buffer");
        *v = (long) i;
    }

    // Remove 50 chars from the middle
    for (size_t i = 25, l = buffer_size_long(&b) - 25; i < l; i++) {
        bool b_remove = buffer_remove_long(&b, 25, 1);
        expect(b_remove, "Failed to remove from buffer");
    }

    // Check first 25 ints
    for (size_t i = 0; i < 25; i++) {
        long *n = buffer_get_long(&b, i);
        expect(*n == 99 - i, "Unexpected value");
    }

    // Check last 25 ints
    for (size_t i = 25; i < 25; i++) {
        long *n = buffer_get_long(&b, i);
        expect(*n == 99 - i, "Unexpected value");
    }

    buffer_destroy_long(&b);
    succeed;
}

test sparsearray_test() {
    sparsearray s;
    bool s_init = sparsearray_create(&s, 10, 10);
    expect(s_init, "Failed to create sparsearray");

    // Add 100 keys
    for (uint64_t key = 0; key < 100; key++) {
        char *v = sparsearray_put(&s, key, sizeof(char));
        expect(v != NULL, "Failed to insert into sparsearray");
        *v = 65 + (key % (127 - 65));
    }

    /*// Double values
    for (uint64_t key = 0; key < 100; key++) {
        char v = *(char *) sparsearray_get(&s, key, sizeof(char));
        char *dv = sparsearray_put(&s, key, sizeof(char));
        *dv = 65 + ((v * 2) % (127 - 65));
    }*/

    for (uint64_t key = 0; key < 10; key++) {
        bool s_remove = sparsearray_remove(&s, key, sizeof(char));
        expect(s_remove, "Failed to remove from sparsearray");
    }

    // Print values
    for (size_t i = 0, l = sparsearray_count(&s); i < l; i++) {
        uint64_t key = sparsearray_key(&s, i);
        char *v = sparsearray_value(&s, i, sizeof(char));
        expect(*v == (65 + (key % (127 - 65))), "Unexpected value");
    }

    sparsearray_destroy(&s);
    succeed;
}

uint64_t hash_char_ptr(char *c) {
    return *c;
}

bool equals_char_ptr(char *a, char *b) {
    return *a == *b;
}

test hashtable_test() {
    hashtable h;
    bool h_init = hashtable_create(&h, 10, 10, 10);
    expect(h_init, "Failed to create hashtable");

    // Add 100 keys
    for (uint64_t i = 0; i < 127 - 65; i++) {
        char c = 65 + i;
        uint64_t v = 100 - i;
        bool h_set = hashtable_put(&h, (uint64_t (*)(void *)) hash_char_ptr, (bool (*)(void *, void *)) equals_char_ptr, &c, sizeof(char), &v, sizeof(uint64_t));
        expect(h_set, "Failed to insert into hashtable");
    }

    // Double values
    for (uint64_t i = 0; i < 127 - 65; i++) {
        char c = 65 + i;
        uint64_t *v = hashtable_get(&h, (uint64_t (*)(void *)) hash_char_ptr, (bool (*)(void *, void *)) equals_char_ptr, &c, sizeof(char), sizeof(uint64_t));
        expect(v != NULL, "Failed to get from hashtable");
        *v = (*v) * 2;
    }

    // Remove some keys
    char *keys = "azAZ";

    for (size_t i = 0; i < strlen(keys); i++) {
        char c = keys[i];
        bool h_remove = hashtable_remove(&h, (uint64_t (*)(void *)) hash_char_ptr, (bool (*)(void *, void *)) equals_char_ptr, &c, sizeof(char), sizeof(uint64_t));
        expect(h_remove, "Failed to remove from hashtable");
    }

    // Print values
    hashtable_iterator it = hashtable_iterate(&h);

    while (hashtable_iterate_next(&it, sizeof(char), sizeof(uint64_t))) {
        char *key = hashtable_iterate_key(&it);
        uint64_t *value = hashtable_iterate_value(&it, sizeof(char));
        expect(*value == 2 * (100 - (*key - 65)), "Unexpected value");
    }

    hashtable_destroy(&h);
    succeed;
}

uint64_t hash_char(char c) {
    return c;
}

bool equals_char(char a, char b) {
    return a == b;
}

HASHTABLE_REGISTER_TYPE(ci, char, uint64_t, hash_char, equals_char)

test hashtable_typed_test() {
    hashtable_ci h;
    bool h_init = hashtable_create_ci(&h, 10);
    expect(h_init, "Failed to create hashtable");

    // Add 100 keys
    for (uint64_t i = 0; i < 127 - 65; i++) {
        char c = 65 + i;
        uint64_t v = 100 - i;
        bool h_put = hashtable_put_ci(&h, c, v);
        expect(h_put, "Failed to insert into hashtable");
    }

    // Double values
    for (uint64_t i = 0; i < 127 - 65; i++) {
        char c = 65 + i;
        uint64_t *v = hashtable_get_ci(&h, c);
        expect(v != NULL, "Failed to get from hashtable");
        hashtable_put_ci(&h, c, *v * 2);
    }

    // Remove some keys
    char *keys = "azbngGNBAZ";

    for (size_t i = 0; i < strlen(keys); i++) {
        bool h_remove = hashtable_remove_ci(&h, keys[i]);
        expect(h_remove, "Failed to remove from hashtable");
    }

    // Print values
    hashtable_iterator_ci it = hashtable_iterate_ci(&h);

    while (hashtable_iterate_next_ci(&it)) {
        char *key = hashtable_iterate_key_ci(&it);
        uint64_t *value = hashtable_iterate_value_ci(&it);
        expect(*value == 2 * (100 - (*key - 65)), "Unexpected value");
    }

    hashtable_destroy_ci(&h);
    succeed;
}

test string_test() {
    // Test equality.
    string *a = string_create(3);
    a = string_append_chars(a, 3, "hej");
    uint8_t aao[6] = { 0xc3, 0xa5, 0xc3, 0xa4, 0xc3, 0xb6 };
    a = string_append_chars(a, sizeof(aao), aao);

    string *b = string_create(3);
    uint8_t hejaao[9] = { 'h', 'e', 'j', 0xc3, 0xa5, 0xc3, 0xa4, 0xc3, 0xb6 };
    b = string_append_chars(b, sizeof(hejaao), hejaao);

    expect(string_equals(a, b), "Strings should equal.");

    // Test inequality.
    string *a2 = string_create(3);
    a2 = string_append_chars(a2, 3, "hej");

    string *b2 = string_create(3);
    b2 = string_append_chars(b2, sizeof(hejaao), hejaao);

    expect(!string_equals(a2, b2), "Strings shouldn't equal.");

    string_free(a);
    string_free(a2);
    string_free(b);
    string_free(b2);
    succeed;
}

int main() {
    tests_start("Utilities");
    test_run(buffer_test);
    test_run(buffer_typed_test);
    test_run(sparsearray_test);
    test_run(hashtable_test);
    test_run(hashtable_typed_test);
    test_run(string_test);
    tests_finish;
    getchar();

    return EXIT_SUCCESS;
}