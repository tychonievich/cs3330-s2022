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
{#file files/cachesim1.c}
and its associated header file
{#file files/cachesim1.h}

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

You can
{#file files/cachesim1_test.c}

In case you've forgotten, you can compile two C files together and then run them using a command invocation like

```bash
clang cachesim1.c cachesim1_test.c -o cachesim1_tester
./cachesim1_tester
```
