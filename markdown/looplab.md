---
title: Looplab
...

# Task

0.  We recommend doing this lab on Linux (a department machine or in a VM). If you do not, then
    see the [compatiblity note](#compat) below.
    
    If you experience issues with "Illegal instruction" errors either use a department machine
    (via NX or via [SSHing into portal.cs.virginia.edu](../../tips/ssh.html)) or remove the GCC7 and Clang
    versions of sum from `sum_benchmarks.c`.

1.  Download the lab tarball [here](files/looplab.tar). Extract the tarball.

2.  Build the lab with `make` and run the produced program `./sum`. This will
    benchmark 4 different functions that equivalent to the following C function:

        unsigned short sum_C(long size, unsigned short * a) {
            unsigned short sum = 0;
            for (int i = 0; i < size; ++i) {
                sum += a[i];
            }
            return sum;
        }

3.  Create a copy of `sum_simple.s`, which contains a commented assembly implementation of
    the above sum function, called `sum_unrolled2.s`. Modify this copy to rename
    the sum function to `sum_unrolled2` and unroll the loop to
    handle two elements per iteration. You do not need to handle
    `size`s that are not multiples of 16.

    Add the resulting `sum_unrolled2` function to `sum_benchmarks.c`,
    recompile with `make`, then observe how much faster the unrolled version
    is by running `./sum`.

4.  Repeat the previous step, but unroll to handle 4 elements per iteration (using the name `sum_unrolled4`).

5.  Create a copy of your `sum_unrolled4.s` called `sum_multiple_accum.s`. Rename
    the function in this copy to `sum_multiple_accum` and modify it to use multiple accumulators.
    (That is, multiple variables to store intermediate results, like in the example from 18 April's lecture
    or Figure 5.21 in the textbook.)
    Make sure you obey the calling convention when choosing where to store
    the additional accumulators.
    Add this to `sum_benchmarks.c` and observe how much faster it is than the
    unrolled version.

6.  In `sum_benchmarks.c`, create a copy of `sum_C` called `sum_multiple_accum_C` and modify it to use 
    multiple accumulators like in your assembly solution. Compare its performance to the assembly
    version.

7.  In a text file called `times.txt` report the performance for the naive assembly version,
    and each of the versions you created on the largest size tested.

    If possible and you have time, we suggest testing on multiple machines (e.g. your laptop and a department machine).

8.  Run `make looplab-submit.tar` to create an archive of all your `.s` files and the text file. Submit
    this file to [the submission site](https://kytos.cs.virginia.edu/cs3330/). If you are working remotely
    on a department machine, our guide to [using SSH and SCP or other file transfer tools](../../tips/ssh.html) may be
    helpful.

# Files in the tarball

*  sum_benchmarks.c --- file containg a list of versions of sum function to time.
*  sum_simple.s --- a simple assembly implementation of the sum code.
*  sum_gcc7_O3.s, sum_clang6_O.s --- versions of the above C code compiled with
   various compilers and various optimization flags
*  sum_main.c --- main function that times several version of the above C code.
*  timing.c, timing.h --- internal code used for timing
*  sum.h --- various definitions used by our timing, sum code
*  Makefile --- instructions to allow `make` to build the testing binary

# The supplied version of sum

The 4 versions we have supplied are:

*  `sum_C` --- This is the above code compiled on your machine. If you didn't change the Makefile, on the department machines, this will use GCC version 4.9 with the options `-O2 -msse4.2`. On your machine it will use whatever compiler `gcc` is with `-O2 -msse4.2`
*  `sum_simple` --- This is the assembly code in `sum_simple.s`. This is an assembly implementation
*  `sum_clang6_O` --- This is the above C code compiled with Clang version 6.0.0 with the options `-O -mavx2` (with the function renamed). The assembly code is in `sum_clang6_O.s`
*  `sum_gcc7_O3` --- This is the above C code compiled with GCC version 7.1.0 with the options `-O3 -mavx2` (with the function renamed). The assembly code is in `sum_gcc7_O3.s`

# Compatability note {#compat}

OS X requires that function names have an additional leading underscore in assembly. So, the supplied assembly files will not work on OS X. The easiest thing to do is use Linux for the lab (either via SSH or via a VM). Alternately, you can modify the assembly files to add an `_` before the function names (e.g. changing `sum_simple:` to `_sum_simple:` and `.global sum_simple` to `.global _sum_simple`).

On OS X, the assmebly code we have supplied from Clang and GCC uses directives that the default OS X assembler does not support. You can deal with this by moving the corresponding assembly files out of the directory (so the Makefile doesn't try to assemble them) and editing the function list in `sum_benchmarks.c` to not list them. (They are not needed to do the lab; they're just provided for comparison.)

If you experience issues with "Illegal instruction" errors, this means that your processor is a little older than we expected.
Either use a department machine (via NX or via [SSHing into portal.cs.virginia.edu](../../tips/ssh.html)) or remove the GCC7 and Clang
versions of sum from `sum_benchmarks.c`.

# Dealing with large output

You can redirect the output of `./sum` to a file using something like

    ./sum > output.txt

Then open `output.txt` in a text editor.

# No performance improvement?

Make sure you that your loop unrolling implementation does not increment the index `i` more than needed.

It is possible that, due to the simple nature of addition, that you will hit the latency bound rather quickly:
-  On a relatively recent Intel processor (Sandy Bridge or later, but not Atom), we expect the best performance without multiple accumulators to be about 1 cycle/element, and with multiple accumulators around .5 cycles/element. However, modern processors often run their cores at variable clock rates, while the cycle counter our timing code uses runs at a constant clock rate that is equal to each core's normal clock rate, but does not change as core clock rates are increased or decreased. Because of this, it may be the case that our timing script reports less than 1 cycle/element when the true performance is 1 or more cycles/element (the clock we're using to measure is slower than the core's clock).
-  It appears that some of the machines that run NX can only do about 1 cycle per element (probably due to limits on the number of loads per cycle); the portal machines seem to generally not be so limited.
-  We have not tested extensively, but we believe most non-very-old AMD processors can perform better than 1 cycle/element with multiple accumulators.

# Appendix: Timing

## Cycle counters

The timing code we have supplied uses the `rdtsc` (ReaD Time Stamp Counter) instruction to measure the performance
of the function. Historically, this accessed a counter of the number of processor clock cycles. On current generation
processors, where different processor cores have different clock rates and clock rates vary to save power,
that is no longer how `rdtsc` works. On modern systems, `rdtsc` accesses the number of cycles of a counter that
counts at a constant rate *regardless of the actual clock speeds of each core*. This means that the cycle counter
reliably measures "wall clock" time rather than actually measuring the number of cycles taken.

Since clock rates vary on modern processors, measurements of wallclock time do not have an obvious correlation
to number of clock cycles. A particular problem are processor features like
[Intel Turbo Boost](https://en.wikipedia.org/wiki/Intel_Turbo_Boost) or
[AMD Turbo Core](https://en.wikipedia.org/wiki/AMD_Turbo_Core). (These might generally
be called "dynamic overclocking".)
In these cases, processor cores briefly
operate at **faster than the normal maximum clock rate**. This means that microbenchmarks like ours my make
the processor appear faster than it would be under normal operation --- e.g., if we needed to compute sums repeatedly
over a period of time. The cycle counter generally counts clock cycles at the "normal" sustained clock rate.

## Taking minimums 

The function tries to give the approximate minimum timing, ignoring temporary effects like moving
arrays into cache or other things running on the system. To do this, it runs the function until:

-  It has run for at least 250 million cycles; and
-  Either:
    -  The 10 shortest times are within .5% of each other; or
    -  200 attempts have been made.

It then returns the 5th shortest time (ordinarily within .5% of the shortest time).
