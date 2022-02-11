---
title: Cache Simulator Homework
...

This homework builds off of the [associated lab](cachesim1.html) and cannot be completed without completing that lab first.

If you are unsure of your cachesim1 code, my solution can be downloaded as a compiled Linux binary [cachesim1.o](files/cachesim1.o)

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
| 70%        | Basics |
| 10%        | Writeback vs writethrough |
| 10%        | Multi-byte reads and writes |
| 10%        | Multi-tier caches |


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
    
    In addition to `tag`, the `meta` structure also has a 1-bit `valid` and 1-bit `dirty` field.
    
How do I find a byte in a block of a line of a set of a `Cache *c`{.c}?
:   First, get the `Line` structure:
    `Line myLine = getLine(c, set_index, line_index_in_set);`{.c}.
    
    The block is stored as an array of bytes,
    so I'd use `myLine.block[block_offset]` to get a particular byte out of it.
    Note that this array's length is determined by `c->block_bits`.

