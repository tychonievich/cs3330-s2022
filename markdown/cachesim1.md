---
title: Cache Simulator Lab
...

Write a C file `cachesim1.h` that

1. Assumes the existence of some macro definitions prior to its inclusion.
    
    - either `#define WRITEBACK`{.c} or `#define WRITETHROUGH`{.c}
    
    - `#define ADDRESS_BITS 48`{.c} or another between 32 and 64
        
        This is the usable bits of an address. For example, if the `ADDRESS_BITS` is 48
        then `0x0123456789abcdef` and `0x456789abcdef` refer to the same address.
    
    - `#define BLOCK_BITS 5`{.c} or another number between 0 and 8

        This is length of a block offset,
        meaning each block contains $2^{\text{BLOCK\_BITS}}$ bytes.
    
    - `#define WAY_BITS 2`{.c} or another number between 0 and 4

        This is $\log_2(w)$ where $w$ is the number of lines per set, also called the "way" of the cache.
        If `WAY_BITS` is 0, then the cache is direct-mapped.
    
    - `#define SETS_BITS 5`{.c} or another number between 0 and 16
    
        This is the length of a set index,
        meaning the cache contains $2^{\text{SETS\_BITS}}$ sets.
        
        If `SETS_BITS` is 0, then the cache is fully-associative.

    You should not set these macros yourself. They will be set prior to the inclusion of `cachesim1.h`.

2. Assumes the type names `u64` for an unsigned 64-bit integer and `u8` for a single byte.
    You should use these type names rather than `unsigned long long`{.c} or the like.

In this file, you should

1. Define some more useful derivative constants

    1. `#define BLOCK (...)`{.c} where `...` is an expression equal to the number of bytes per block.
    1. `#define WAY (...)`{.c} where `...` is an expression equal to the number of ways per set.
    1. `#define SETS (...)`{.c} where `...` is an expression equal to the number of sets per cache.
    1. `#define TAG_BITS (...)`{.c} where `...` is an expression equal to the number of tag bits in an address.
    
    Keep the parentheses in the `#define`{.c} lines, they are important for proper macro evaluation.

1. Define address-parsing functions.

    ````c
    /// The block offset of the given address
    int get_offset(u64 address) { ... }
    
    /// The set index of the given address; 0 if a fully-associative cache
    int get_index(u64 address) { ... }
    
    /// The tag of the given address
    u64 get_tag(u64 address) { ... }
    ````
    
    These may resemble the `getbits` task from [the bit fiddling homework](bitfiddle.html).

1. Implement the logic needed for [Tree-PLRU](tree-plru.html).

    This consists of two functions:
    
    - `newLRU` is given the old PLRU-tracking bits and which line of the set was just accesses and returns the updated PLRU-tracking bits.
    - `LRUindex` is given PLRU-tracking bits and returns the index of the pseduo-least recently used line in the set.
    
    Guard this with an `#if` so it only gets defined if `WAY_BITS` is not zero.
    
    ````c
    #if WAY_BITS

    /// update Tree-PLRU tracking bits based on an access of the given line
    int newLRU(int oldLRU, int lineindex) { ... }

    /// Given Tree-PLRU tracking bits, return index of least-recently-used line
    int LRUindex(int lru) { ... }

    #endif 
    ````
    
