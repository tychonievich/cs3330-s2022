---
title: Cache Simulator Homework
...


> Change log
>
> - <small>2022-02-10 12:59</small> added `cachesim1.o` file
> - <small>2022-02-15 09:40</small> added debugging section and more coding help
> - <small>2022-02-15 10:05</small> updated `cachesim2.tar` to have more forgiving tests for oversized addresses
> - <small>2022-02-15 11:51</small> added more coding help
> - <small>2022-02-15 13:41</small> that referred to a field `valid` when it should have been `live` instead
> - <small>2022-02-15 17:35</small> updated `cachesim2.tar` to fix mistakes introduced in previous update

This homework builds off of the [associated lab](cachesim1.html) and cannot be completed without completing that lab first.

If you are unsure of your cachesim1 code, my solution can be downloaded[^using] as a compiled Linux binary [cachesim1.o](files/cachesim1.o).

[^using]:
    To use my cachesim1.o:

    - Rename your `cachesim1.c` to be `cachesim1.c.backup` instead
    - Download my `cachesim1.o`, as e.g. `curl http://www.cs.virginia.edu/luther/3330/S2022/files/cachesim1.o > cachesim1.o`{.sh}
    - Change the `Makefile` to include `cachesim1.o` at the end of the `objects :=` line; i.e. `objects := $(patsubst %.c,%.o,$(wildcard *.c)) cachesim1.o`{.makefile}
    - The reference assumes `global_init` has been called *after* the global variables are set, so you'll need to do that to get correct results


# Task

You must work alone on this homework.

Download [cachesim2.tar](files/cachesim2.tar);
extract it;
move you `cachesim1.c` from the lab into the folder;
and complete `cachesim2.c`.

{#file files/cachesim2.c}

The provided file has default implementations that ignore cache and go directly to RAM, all of which you should replace for full credit.

There are several levels of completion possible:

| Percentage | Functionality |
|:----------:|---------------|
| 90%        | Basics |
| 10%        | Writeback vs writethrough |
| 5%        | Multi-byte reads and writes |
| 5%        | Multi-tier caches |

Note: earning 110% is possible if you do them all

## Basics

Implement `getByte` and `setByte` for single-level writethrough caches.
Some things to keep in mind:

- Writethrough caches backed by RAM should send the entire written-to block to `writeToRAM`, not just the new byte

- It's a cache hit if *any* line in a set is *both* (a) live and (b) has the same tag as the address.

- Update the most-recently used line in the PLRU for both hits and misses on both reads and writes.

- You can do the address parsing and PLRU handling using your lab code; for example, I'd expect to see code snippets like
    - `tag = get_tag(address);`{.c}
    - `lineToReplace = idxFromLRU(c->plru[setIndex]);`{.c}
    - `c->plru[setIndex] = newLRU(c->plru[setIndex], indexOfAccessedLineInSet);`{.c}
    - ... and so on

## Writeback vs writethrough

Also support writeback caches:
if the cache `isWriteback`
track `dirty` bits
and only `writeToRAM` when evicting a dirty line from the cache.

## Multi-byte values

Also support the …Block and …Long functions.

For …Long functions, if two cache lines are accessed
please access the smaller-address line first so that you can match our test cases.

## Multi-tier caches

Also support non-`NULL` `backing` caches.

- You may assume that `c->backing->block_bits >= c->block_bits`.
- If you are using the global variables from cachesim1 to parse addresses, make sure you change them appropriately when moving from one cache level to another.
- We will test all combinations of writing policy (all -through, all -back, -through backed by -back, -back backed by -through).


# Testing

We've provided a testing program that reads test case input files and prints results,
along with example test cases and results.
You can run the full suite of tests with `make test`.
To get only summary information, not full details, run `make test >/dev/null`.
If you want to run your own test, try `./cachsim2_test yourinputfile`.

The tests we provide cover many cases, but not all.
For example, there's no test with an L3 cache, nor a writeback L2 and writethrough L1, nor multibyte accesses in a multitier cache.
For 100% credit, you'll need to make sure your cache works for all of those cases too.


## Debugging

If `make test` is saying something like `WRONG keys/basic-01.txt` you probably want to try that test by running `./cachesim2_test tests/basic-01.txt` to see what your code is doing. 

The output of the tester is

- One line for each RAM action and cache access return value from your cache
- A printout of the entire cache

You can compare this to the expected output in the corresponding `keys/` file.



# Coding help

The `Cache` structure is doing some fancy things under the hood to be able to handle multiple cache sizes with a single structure. You are welcome to read how that is happening in `cachesim2_lib.c` if you want, but we recommend just using it instead.

What size cache is `Cache *c`{.c}?
:   `c->set_bits`{.c}, `c->way_bits`{.c}, and `c->block_bits`{.c} have the same meaning for this cache that the corresponding global variables did in [the lab](cachesim1.html).

What is the Tree-PLRU tracking data in `Cache *c`{.c}?
:   `c->plru`{.c} is an array of this, one entry per set.
    So the Tree-PLRU information for set `i` can be accessed anc changed by using `c->plru[i]`{.c}.

What is the tag of a line in `Cache *c`{.c}?
:   Lines are identified by a line within a set.
    If I'm looking at set `set_index` and its line `line_index`, I'd use
    `Line myLine = getLine(c, set_index, line_index_in_set);`{.c}.
    
    The tag of the line is part of the `LineMetadata`,
    so I'd get the tag as `myLine.meta->tag`{.c}
    and set it as `myLine.meta->tag = newTag;`{.c}
    
    In addition to `tag`, the `meta` structure also has a 1-bit `live` and 1-bit `dirty` field.
    
How do I find a byte in a block of a line of a set of a `Cache *c`{.c}?
:   First, get the `Line` structure:
    `Line myLine = getLine(c, set_index, line_index_in_set);`{.c}.
    
    The block is stored as an array of bytes,
    so I'd use `myLine.block[block_offset]` to get a particular byte out of it.
    Note that this array's length is determined by `c->block_bits`.

How do I use `void readFromRAM(u64 address, size_t bytes, u8* data)`{.c}?
:   1. Call it at most once per cache access
        - only on a cache miss
        - and only if `backing == NULL`, otherwise use `getBlock(backing, ...)` instead
    2. With `bytes` being the number of bytes per block
    3. And `data` being where you want to store the result -- i.e., `myline.block`
    4. And `address` being the starting address of the block
        - This is like the given address, except the unused and block offset bits are zero.

How do I use `void writeToRAM(u64 address, size_t bytes, const u8* data)`{.c}?
:   1. Call it at most once per cache access
        - for write-through, immediately after modifying a block
        - for write-back, immediately before evicting a dirty live line
        - and only if `backing == NULL`, otherwise use `setBlock(backing, ...)` instead
    2. With `bytes` being the number of bytes per block
    3. And `data` being where you want to copy to RAM -- i.e., `myline.block`
    4. And `address` being the starting address of the block
        - This is like the given address, except the unused and block offset bits are zero.

What should `u8 getByte(Cache *c, u64 address)`{.c} do?
:   1. parse out the parts of the address, like in the lab
    2. check every line of the appropriate set to see if there's a cache hit
    3. if there is a cache miss,
        a. pick the least-recently-used line to evict, as defined by the PLRU information and the lab code
        b. if the line is dirty and live, write it out before eviction
        c. load a new block and tag into the line
    4. mark the line (either the hit line or the new line) as the most-recently used, using the lab code
    5. return the appropriate byte of the block of the line (either the hit line or the new line)

What should `void setByte(Cache *c, u64 address, u8 value)`{.c} do?
:   What `getByte` does, except:
    
    - modify the byte of the block instead of returning it
    - either mark the line as dirty (write-back) or write the entire block to the backing (write-through)

What should `void getBlock(Cache *c, u64 address, u8 *dst, size_t bytes)`{.c} do?
:   What `getByte` does, except:

    - instead of returning one byte, copy `bytes` bytes from the line's block into the `dst` array

What should `void setBlock(Cache *c, u64 address, const u8 *src, size_t bytes)`{.c} do?
:   What `setByte` does, except:
    
    - instead of modifying one byte, modify `bytes` bytes in the block to be equal to values in the `src` array

What should `u64 getLong(Cache *c, u64 address)`{.c} do?
:   It's like `getBlock`, but
    
    - the 8 bytes it reads are a (little-endian) `u64` that is returned
    - the bytes might span two lines instead of just one

What should `void setLong(Cache *c, u64 address, u64 value)`{.c} do?
:   It's like `setBlock`, but
    
    - the 8 bytes it writes are from a (little-endian) `u64`
    - the bytes might span two lines instead of just one

Why am I getting a segfault?
:   There are many possible causes, but a few likely reasons:
    
    - Confusing the `u64 address`, a value we are working with, with the C/C++ notion of "address"/"pointer".
        The (cache) address of a line in the cache is a value you are given or compute from the parts of a (cache)  address,
        not something you can find with the `&` (C) address-of operator.
        
        If your code has `&` in any of the \_etbyte or \_etBlock functions, there's a high chance you are doing something wrong.
    
    - Confusing the block size (the length of an array, derived from `c->block_bits`)
        and the C/C++ notion of "sizeof" (the bytes used by the language to store a datatype).

        If your code has `sizeof` in any of the \_etbyte or \_etBlock functions, there's a high chance you are doing something wrong.

    - Confusing what `=` does when given pointers as values.
        `myline.block = something`{.c} does *not* copy values from array `something` into array `myline.block`;
        rather, it changes the block pointer inside the line, which is supposed to indicate part of the cache, to point to something else instead.
        
        To copy arrays, use a loop (`for(size_t i=0; i<size; i+=1) myline.block[i] = something[i];`{.c}) or the `memcpy` standard library function from `<string.h>`.
        
        Note that `readFromRAM` and `getBlock` accept as a parameter the array to put data into, so you might be able to avoid most array copies by giving them the final destination array directly.
    
    - Treating a single value like an array.
        In C/C++, a pointer can point to a single value or to an array of values;
        the only way to tell those apart is by how you create them.
        
        In this assignment,
        
        - Any `Cache *` value we provide is a pointer to a single value; so is `myline.metadata`.
        - Any `u8 *` or `u16 *` value we provide is a pointer to an array of values; so `myline.block`, `c->plru`, and the `src` variable in `setBlock` are all pointers to arrays.
        - You shouldn't need to create additional variables of either type, but will need to use those provided as intended.
        - Several placeholder implementations use `&value` to treat `value` like a single-entry array, but you'll need to replace those with appropriate multi-entry arrays instead.
        
        
