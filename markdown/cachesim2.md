---
title: Cache Simulator Homework
...

This homework builds off of the [associated lab](cachesim1.html) and cannot be completed without completing that lab first.

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

- You may assume that `c->backing->blocksize >= c->blocksize`.
- If you are using the global variables from cachesim1 to parse addresses, make sure you change them appropriately when moving from one cache level to another.
- We will test all combinations of writing policy (all -through, all -back, -through backed by -back, -back backed by -through).


# Testing

We've provided a testing program that reads test case input files and prints results,
along with example test cases and results.
You can run the full suite of tests with `make test`.
If you want to run your own test, try `./cachsim2_test yourinputfile`.
