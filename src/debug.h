#pragma once
#include <stdio.h>
#include <assert.h>

/// <summary>
/// Macro to explicitly specify `x` is unused. For example to avoid warning on
/// release build when using `x` in debug macros like `assert`.
/// </summary>
#define unused(x) ((void) (x))

#ifdef _DEBUG
#define DEBUG
#else
#define NDEBUG
#endif

#ifdef DEBUG
#define debug(message, ...) do { fprintf(stderr, "DEBUG %s:%d: " message "\n", __FILE__, __LINE__, ##__VA_ARGS__); } while (0)
#else
#define debug(message, ...) do {} while (0)
#endif
