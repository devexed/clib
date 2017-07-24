# vexlib
Experimental C-library for debugging, testing, and datastructures.

## Contents

* Debug macros
* Minimal testing library
* Array - With length and capacity stored next to the data
* Buffer - With length and capacity stored next to a pointer to the data
* UTF-8 String - Array which contains only valid, [NFD](//en.wikipedia.org/wiki/Unicode_equivalence#Normal_forms) UTF-8
* Sparse array - Array with non-sequential indexes
* Hashtable - Experimentally backed by a sparse array with hashes as indexes (to be properly implemented as a proper hash table)

## Dependencies

* [utf8proc](//github.com/JuliaLang/utf8proc) - For all the heavy UTF-8 lifting
