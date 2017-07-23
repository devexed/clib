/* Simple dynamic string with length included. */
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <utf8proc.h>
#include "array.h"

// A string is an array of 8-bit elements in valid utf8.
typedef array string;

string * string_empty();

string * string_create(size_t);

void string_free(string *);

void string_clear(string *);

string * string_trim(string *);

size_t string_size(string *);

string * string_append_char(string *, uint8_t);

string * string_append_chars(string *, size_t, uint8_t *);

string * string_append_codepoint(string *, uint32_t);

string * string_append_string(string *, string *);

size_t string_iterate(string *, size_t, size_t, int32_t *);

bool string_equals(string *, string *);

uint64_t string_hash_fnv1a(string *);

uint64_t string_hash_loselose(string *);

uint8_t * string_chars(string *);

uint8_t string_decimal_digit(int32_t codepoint);
