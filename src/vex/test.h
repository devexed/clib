#pragma once
#include <stdio.h>

typedef char * test;

/// <summary>
/// Used at the end of every test function to signal the test ended
/// successfully.
/// </summary>
#define succeed return NULL

/// <summary>
/// Used to signal a test failed.
/// </summary>
#define fail(message) do { return message; } while(0)

/// <summary>
/// Fail with message if condition is false.
/// </summary>
#define expect(condition, message) do {                                       \
        if (!(condition)) fail(message);                                      \
    } while(0)

/// <summary>
/// Start running a series of tests.
/// </summary>
#define tests_start(title)                                                    \
    int tests_failed = 0;                                                     \
    do {                                                                      \
        printf("Running tests - " title "...\n");                             \
    } while(0)

/// <summary>
/// Run a single test.
/// </summary>
#define test_run(function) do {                                               \
        char *m = function();                                                 \
        if (m != NULL) {                                                      \
            printf("FAIL - Test " #function " failed: %s\n", m);              \
            tests_failed++;                                                   \
        } else {                                                              \
            printf("OK - Test " #function " succeeded\n");                    \
        }                                                                     \
    } while(0)

/// <summary>
/// Finish running a series of tests, printing the result.
/// </summary>
#define tests_finish do {                                                     \
        if (tests_failed > 0) {                                               \
            printf("%d tests failed\n", tests_failed);                        \
        } else {                                                              \
            printf("All tests succeed\n");                                    \
        }                                                                     \
    } while(0)
