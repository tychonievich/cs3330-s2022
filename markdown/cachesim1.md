---
title: Cache Simulator Lab
...

Least-Recently Used (LRU) cache replacement policies are nice theoretical, but to be effective in an L1 cache they'd need to be very fast, well under a single clock cycle to compute, and they aren't that fast with any reasonable way^[
    When we speak of an 8-way set-associative cache, we mean each set has 8 lines.
    That number 8 is called the "way" of the cache.
].
So instead we use a pseudo-LRU (PLRU) that is simpler and faster, but less accurate, than true LRU.

In this lab and the subsequent homework you'll implement an Tree-PLRU cache simulator in C.

# Task

Complete the following C skeleton file:

- Download: [cachesim1.c](files/cachesim1.c)
- <details><summary>Or view:</summary>

    ````c
    typedef unsigned char u8;
    typedef unsigned long long u64;

    /// These will be defined in a different file
    extern u8 block_bits, way_bits, sets_bits, address_bits;

    /// Called once after each change to the defining extern values above.
    /// we won't test it, but you can use it to set globals like `tag_bits` if you wish.
    void global_init() {
        // optionally, add code here
    }

    /// update Tree-PLRU tracking bits based on an access of the given line
    int newLRU(int oldLRU, int index) {
        return 0; // fix me
    }

    /// Given Tree-PLRU tracking bits, return index of least-recently-used line
    int idxFromLRU(int lru) {
        return 0; // fix me
    }

    /// Given an address, return the block offset from it
    int get_offset(u64 address) {
        return 0; // fix me
    }
    /// Given an address, return the set index from it
    int get_index(u64 address) {
        return 0; // fix me
    }
    /// Given an address, return the tag from it
    u64 get_tag(u64 address) {
        return 0; // fix me
    }
    ````
    
    </details>

You may either work alone or with a buddy in this lab.
Buddy programming is where two people work side-by-side,
each creating similar programs while talking together to help one another out.
In well-running buddy programming each buddy is speaking about equally,
describing what they are writing next or how they are testing what they have written.
Buddy programming usually results in similarly-designed but non-identical programs.

This lab involves implementing Tree-PLRU. See also our [Tree-PLRU writeup](tree-plru.html) for more.

This lab involves accessing specific bits of an integer.
You've done this before, as the `getbits` task from [the bit fiddling homework](bitfiddle.html).


# Testing

If you've correctly implemented `cachesim1.c`, you should be able to compile it with `cachesim1_test.c` and run it to get the output `Passed 100/100 tests`.

- Download: [cachesim1_test.c](files/cachesim1_test.c)
- <details><summary>Or view:</summary>

    ```c
    #include <stdio.h>
    #include <stdarg.h>

    typedef unsigned char u8;
    typedef unsigned long long u64;

    // forward declaration of functions defined in cachesim1.c
    void global_init();
    int newLRU(int oldLRU, int index);
    int idxFromLRU(int lru);
    int get_offset(u64 address);
    int get_index(u64 address);
    u64 get_tag(u64 address);

    /// the global variables we use to control cachesim1.c
    u8 block_bits, way_bits, sets_bits, address_bits;

    /// a wrapper function for displaying only one error message of a given type
    int errmsg(int idx, const char *msg, ...) {
        static u64 mask = 0;
        if (mask & (1<<idx)) return 0; // already reported one of these errors
        mask |= (1<<idx);
        
        va_list myargs;
        va_start(myargs, msg);
        int ret = vprintf(msg, myargs);
        va_end(myargs);
        return ret;
    }

    /// a compact, if confusing, testing wrapper
    typedef struct {
        u8 bb, wb, sb, ab;
        u64 addr; int off, idx; u64 tag;
        int lru1, idx1, idx2, lru2;
    } tcase;

    /// ditto
    int runTest(tcase *test) {
        block_bits = test->bb;
        sets_bits = test->sb;
        way_bits = test->wb;
        address_bits = test->ab;
        int correct = 0;

        // check the globals
        global_init();
        
        // check parts of address
        if (get_offset(test->addr) == test->off) correct += 1;
        else errmsg(0, "Block Offset wrong (e.g. for 0x%llx it should be 0x%x not 0x%x)\n", test->addr, test->off, get_offset(test->addr));
        if (get_index(test->addr) == test->idx) correct += 1;
        else errmsg(1, "Set Index wrong (e.g. for 0x%llx it should be 0x%x not 0x%x)\n", test->addr, test->idx, get_index(test->addr));
        if (get_tag(test->addr) == test->tag) correct += 1;
        else errmsg(2, "Tag wrong (e.g. for 0x%llx it should be 0x%llx not 0x%llx)\n", test->addr, test->tag, get_tag(test->addr));
        
        // check LRU processing
        if (idxFromLRU(test->lru1) == test->idx1) correct += 1;
        else errmsg(3, "Wrong psuedo-LRU line number (e.g. for 0x%x it should be 0x%x not 0x%x)\n", test->lru1, test->idx1, idxFromLRU(test->lru1));
        if (newLRU(test->lru1, test->idx2) == test->lru2) correct += 1;
        else errmsg(4, "Wrong update to LRU (e.g. accessing line 0x%x with PLRU 0x%x should make PRLU 0x%x not 0x%x)\n", test->idx2, test->lru1, test->lru2, newLRU(test->lru1, test->idx2));
        
        return correct;
    }

    /// example test cases with 30ish freebies
    tcase tests[] = {
        {6,3,8,48, 0x0,0x0,0x0,0x0, 0,0,7,0},
        {6,3,8,48, 0xFFFFFFFFFFFFFFFF,0x3F,0xFF,0x3FFFFFFFF, 0,0,0,0xB},
        {8,3,6,48, 0x0123456789ABCDEF,0xEF,0xD,0x1159e26af, 0xB,4,0,0xB},
        {6,3,6,48, 0x0123456789ABCDEF,0x2F,0x37,0x456789abc, 0xB,4,1,0x3},
        {6,3,6,64, 0x0123456789ABCDEF,0x2F,0x37,0x123456789abc, 0x0,0,7,0x0},
        {0,3,0,48, 0x0123456789ABCDEF,0x0,0x0,0x456789abcdef, 0x0,0,6,0x40},
        {8,3,0,32, 0x0123456789ABCDEF,0xef,0x0,0x89abcd, 0x40,0,5,0x44},
        {8,3,20,48, 0x1abe1edadebac1e,0x1e,0xdebac,0xe1eda, 0x44,0,4,0x64},
        {1,3,12,30, 0x15bd5be1d,0x1,0xf0e,0xdead, 0x64,0,3,0x65},
        {0,3,0,64, 0,0,0,0, 0x65,7,2,0x75},
        {0,3,0,64, 0,0,0,0, 0x75,7,1,0x77},
        {0,3,0,64, 0,0,0,0, 0x77,7,0,0x7F},
        {0,2,0,64, 0,0,0,0, 0x7,3,1,0x5},
        {0,2,0,64, 0,0,0,0, 0x6,1,1,0x5},
        {0,2,0,64, 0,0,0,0, 0x5,3,1,0x5},
        {0,2,0,64, 0,0,0,0, 0x4,0,1,0x5},
        {0,2,0,64, 0,0,0,0, 0x3,2,1,0x1},
        {0,2,0,64, 0,0,0,0, 0x2,1,1,0x1},
        {0,2,0,64, 0,0,0,0, 0x1,2,1,0x1},
        {0,2,0,64, 0,0,0,0, 0x0,0,1,0x1},
    };

    /// testing harness wrapper
    int main(int argc, char *argv[]) {
        int correct = 0;
        int outof = 0;
        for(int i=0; i<sizeof(tests)/sizeof(tcase); i+=1) {
            outof += 5;
            correct += runTest(&tests[i]);
        }
        printf("Passed %d / %d tests\n", correct, outof);
    }
    ```
    
    </details>

In case you've forgotten, you can compile two C files together and then run them using a command invocation like

```bash
clang cachesim1.c cachesim1_test.c -o cachesim1_tester
./cachesim1_tester
```
