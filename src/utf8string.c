#include <string.h>
#include <assert.h>
#include "utf8string.h"
#include "debug.h"

string * string_empty() {
    return array_create(sizeof (uint8_t) * 16);
}

string * string_create(size_t capacity) {
    return array_create(capacity);
}

void string_free(string *s) {
    array_free(s);
}

void string_clear(string *s) {
    array_pop(s, array_size(s));
}

string * string_trim(string *s) {
    return array_trim(s);
}

size_t string_size(string *s) {
    return array_size(s);
}

string * string_append_char(string *s, uint8_t c) {
    // Only allow ASCII characters to be appended one by one.
    assert(c < 128);

    return array_push(s, sizeof(uint8_t), &c);
}

string * string_append_chars(string *s, size_t utf8_chars_size, uint8_t *utf8_chars) {
    // Return NULL early to simplify chaining array operations.
    if (s == NULL) return NULL;

    // Decompose the characters in NFD.
    utf8proc_option_t options = UTF8PROC_STABLE | UTF8PROC_DECOMPOSE;

    // Accomodate the new decomposed codepoint array in the string. The
    // requried size is first computed by doing a decompose with a zero buffer,
    // which returns the number of needed codepoints. A byte is added to
    // satisfy `utf8proc_reencode`s requirement for one extra byte at the end
    // of the buffer.
    size_t append_codepoints_size =
        utf8proc_decompose(utf8_chars, utf8_chars_size, NULL, 0, options) *
        sizeof(utf8proc_int32_t) + 1;
    size_t original_size = string_size(s);
    string *new_string = array_accomodate(s, original_size + append_codepoints_size);
    
    if (new_string == NULL) return NULL;

    // Decompose the 32 bit codepoints directly into the string.
    utf8proc_int32_t* decomposed_codepoints = new_string->data + original_size;
    utf8proc_ssize_t decomposed_codepoint_count = utf8proc_decompose(
        utf8_chars, utf8_chars_size,
        decomposed_codepoints, append_codepoints_size,
        options);
    
    // Error while decomposing.
    if (decomposed_codepoint_count < 0)
        return NULL;

    assert(decomposed_codepoint_count <= append_codepoints_size);

    // Reencode the codepoints in place to UTF-8.
    utf8proc_ssize_t byte_count = utf8proc_reencode(decomposed_codepoints, decomposed_codepoint_count, options);

    if (byte_count < 0)
        return NULL;

    new_string->size += byte_count;

    // At this point the string will most likely have a lot of extra space at
    // the end, since 32-bit codepoints have now been reencoded to UTF-8.
    return new_string;
}

string * string_append_codepoint(string *s, uint32_t codepoint) {
    assert(utf8proc_codepoint_valid(codepoint));

    // Encode codepoint into utf8 bytes.
    utf8proc_uint8_t codepoint_bytes[4];
    utf8proc_ssize_t codepoint_size = utf8proc_encode_char(codepoint, codepoint_bytes);

    // If code point is valid the codepoint size can not be 0.
    assert(codepoint_size > 0);

    // Append bytes to string
    return array_push(s, codepoint_size, codepoint_bytes);
}

string * string_append_string(string *target, string *source) {
    return array_push(target, array_size(source), array_get(source, 0));
}

size_t string_iterate(string *s, size_t byte_offset, size_t max_bytes_read, int32_t *codepoint) {
    assert(byte_offset < string_size(s));

    // Use utf8proc to get the codepoint at the offset
    const utf8proc_uint8_t *str = array_get(s, byte_offset);
    utf8proc_ssize_t read_count = utf8proc_iterate(str, max_bytes_read, codepoint);

    // Convert negative size_t to 0 as string is expected to contain only valid
    // utf8 data. For more detailed error handling you can use the utf8proc
    // library directly.
    return read_count > 0 ? read_count : 0;
}

bool string_equals(string *a, string *b) {
    assert(a != NULL);
    assert(b != NULL);
    size_t offset = 0;
    size_t size = string_size(a);

    if (size != string_size(b)) return false;

    return memcmp(a->data, b->data, size) == 0;
}

uint64_t string_hash_fnv1a(string *s) {
    uint64_t hash = 14695981039346656037;
    size_t offset = 0;
    size_t size = string_size(s);
    int32_t codepoint;

    while (offset < size) {
        size_t codepoint_size = string_iterate(s, offset, size - offset, &codepoint);
        assert(codepoint_size > 0);
        offset += codepoint_size;
        hash ^= codepoint;
        hash *= 1099511628211;
    }

    return hash;
}

uint64_t string_hash_loselose(string *s) {
    uint64_t hash = 0;
    size_t offset = 0;
    size_t size = string_size(s);
    int32_t codepoint;

    while (offset < size) {
        size_t codepoint_size = string_iterate(s, offset, size - offset, &codepoint);
        assert(codepoint_size > 0);
        offset += codepoint_size;
        hash += codepoint;
    }

    return hash;
}

uint8_t * string_chars(string *s) {
    return array_get(s, 0);
}

uint8_t string_decimal_digit(int32_t codepoint) {
    // The ND category contains contiguous ranges of decimal digits.
    // Here the zeroes of each 0-9 digit sequence are stored.
    static int32_t zeroes[] = {
        0x0030,
        0x0660,
        0x06F0,
        0x07C0,
        0x0966,
        0x09E6,
        0x0A66,
        0x0AE6,
        0x0B66,
        0x0BE6,
        0x0C66,
        0x0CE6,
        0x0D66,
        0x0DE6,
        0x0E50,
        0x0ED0,
        0x0F20,
        0x1040,
        0x1090,
        0x17E0,
        0x1810,
        0x1946,
        0x19D0,
        0x1A80,
        0x1A90,
        0x1B50,
        0x1BB0,
        0x1C40,
        0x1C50,
        0xA620,
        0xA8D0,
        0xA900,
        0xA9D0,
        0xA9F0,
        0xAA50,
        0xABF0,
        0xFF10,
        0x104A0,
        0x11066,
        0x110F0,
        0x11136,
        0x111D0,
        0x112F0,
        0x11450,
        0x114D0,
        0x11650,
        0x116C0,
        0x11730,
        0x118E0,
        0x11C50,
        0x16A60,
        0x16B50,
        0x1D7CE,
        0x1D7D8,
        0x1D7E2,
        0x1D7EC,
        0x1D7F6,
        0x1E950
    };

    // Find which digit sequence the codepoint is in and convert it to its
    // numerical digit value.
    for (size_t i = 0; i < sizeof(zeroes); i++) {
        int32_t zero = zeroes[i];

        if (codepoint >= zero && codepoint <= zero + 9)
            return (uint8_t) (codepoint - zero);
    }

    // This function should only be called with a codepoint in the ND category.
    debug("Unreachable, codepoint U+%04x not in ND category", codepoint);
    assert(false);

    return 0;
}
