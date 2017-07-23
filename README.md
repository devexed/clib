# clib
Experimental C-library for debugging, testing, and datastructures. Contents so far:  

* Debug macros
* Minimal testing library
* Arrays - With length and capacity stored next to the data
* Buffers - With length and capacity stored next to a pointer to the data
* UTF-8 Strings - Arrays which contains only valid, [NFD](https://en.wikipedia.org/wiki/Unicode_equivalence#Normal_forms) UTF-8
* Sparse arrays - Arrays with non-sequential indexes
* Hashtable - Experimentally backed by a sparse array with hashes as indexes, but this will probably have to become more efficient  
