---
title: "SIMD lab"
...

{:.changelog}
Changelog:

{:.changelog}
*  17 Nov 2021: correct comment on `add_AVX` assembly code to match C code
*  17 Nov 2021: correct `_mm_setzero_si256` to `_mm256_setzero_si256`

# Task

0.  Compatibility note: If you have an processor too old to support AVX2, then this lab may not work on your machine. In that case, please use
    department machines such as by [SSHing into portal.cs.virginia.edu or using NX](sshscp.html). **On the department machines, you will
    need to run `module load gcc-7.1.0` before running `make` for this lab.**

0.  In general, this lab deals with vector instructions and their corresponding "intrinsic" functions. There are several sources for information on these:
    *  Below, there is a [brief introduction](#introdution) to SIMD and the intrinsic functions, which should mostly duplicate the lecture material.
    *  We provide a *reference* for vector intrinsic functions you may find useful [here](simdref.html)
    *  The [official Intel documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/) provides a comprehensive list of the intrinsic functions. Our the department machines do not support the AVX512 options, so to use this reference check the "SSE" through "SSE4.2" options and the "AVX" and "AVX2" options (but none of the "AVX512" options).

1.  Download [simdlab.tar](files/simdlab.tar) and extract it.

2.  Read the [brief introduction to SIMD below](#introduction) if you need a refresher of the lecture material on vector instructions.

2.  Read the [explanation of an example SIMD function below](#exampleSIMD). This includes an description of several
    things you will need for the next step:
    *  the [type `__m256i`](#sseTypes)
    *  the [intrinsic functions `_mm256_loadu_si256` and `_mm256_storeu_si256` and `_mm256_add_epi16`](#sumIntrinsics)

2.  Edit `sum_benchmarks.c` to add a function `sum_AVX` that uses vector instrinstics in a very similar way tot he example SIMD function: 
    *  Start by making a copy the `sum_with_sixteen_accumulators` supplied with the tarball.
    *  Change it to store the sixteen accumulators in one of the 256-bit registers rather than sixteen seperate registers, and use vector instructions to manipulate this register. You will primarily use the instrinsic `_mm256_add_epi16` (add packed 16-bit integer values).
    *  See the [detailed explanation](#sumTask) below.

    Add `sum_AVX` to the list of benchmarks in `sum_benchmarks.c`. Compile it by running `make` and then run `./sum` to time it.

3.  Edit `min_benchmarks.c` to add a new function `min_AVX` that does the same thing as the supplied `min_C` function. You can use a strategy very similar to the one used for `sum_AVX`, using the intrinsic function `_mm256_min_epi16`. See the [detailed explanation](#minTask) and descriptions of useful intrinsic functions below below.
    
    Add `min_AVX` to the list of benchmarks in `min_benchmarks.c`, compile it with `make`, then run `./min` to time it.

4.  Edit `dot_product_benchmarks.c` to create a vectorized version of `dot_product_C` called `dot_product_AVX`. See the [detailed explanation](#dotProductTask) below.

5.  (Optional) If you have time, then modify your `dot_product_benchmarks.c` to try to improve the performance of your dot product function using the [advice below](#dotProductOptimize).

6.  Submit whatever you have completed to [the submission site](https://kytos.cs.virginia.edu/cs3330/).

# Compatability note {#compat}

OS X requires that function names have an additional leading underscore in assembly and does not support some assembly directives. So, the supplied assembly files (provided for comparison with good compiler-generated versions) will not work on OS X. The easiest thing to do is use Linux for the lab (either via SSH or via a VM). Alternately, you can:

*  edit the supplied assembly files `sum_clang6_O.s` and `dot_product_gcc7_O3.s` to change the function name in them and remove unsupported assembly directives; or

*  remove these functions from the list of benchmarks in `sum_benchmarks.c`, `dot_product_benchmarks.c` and remove the corresponding assembly files from the Makefile

# SIMD introduction {#introduction}

In this lab, you will we use SIMD (Single Instruction Multiple Data) instructions,
also known as vector instructions, available
on our the department machines in order to produce more efficient versions of several simple functions.

Vector instructions act on vectors of values. For example

    vpaddw %ymm0, %ymm1, %ymm0

uses 256-bit registers, `%ymm0` and `%ymm1` and stores the result in `%ymm0`. But instead of adding two 256-bit integers together,
it treats each register has a "vector" of sixteen 16-bit integers and adds each pair of 16-bit integers.
The instructions we will be using in this lab are part of versions of Intel's AVX (Advaned Vector eXtensions)
to the x86 instruction set. Our machines also support Intel's previous SSE (Streaming Simd Extensions),
which work similarly, but have 128-bit registers instead of 256-bit registers.

Rather than writing assembly directly, we will have you use Intel's Intrinsics functions
to do so. For example, to access the `vpaddw` instruction from C, you will instead call the special
function `_mm256_add_epi16`. Each of these functions will compile into particular assembly instructions, allowing us to
specify that the special vector instructions should be used without also needing to write
all of our code in assembly.

You will create vectorized versions of three functions in the lab.

We believe we have included all the information you need to complete this lab in this lab, but
we also have a more reference-like explanation of the Intel intrinsics [here](simdref.html).

## A note on compiler vectorizations

Compilers can sometimes generate vector instructions automatically. The Makefile we have supplied in this
lab has optimization settings where the compiler on our the department machines will not do this. We believe this will
also be the case with other compilers, but we have not tested all of them.

The purpose of this lab is to familiarize you with how to use vector operations, so you can deal with
more complicated problems where the compiler will not do a good enough job and understand what compilers
are doing better.

# General SIMD reference {#generalReference}

We have tried to include the information about the Intel SSE intrinsic functions
We provide a partial reference to the Intel SSE intrinsic functions [here](simdref.html), which you
may wish to refer to.

In addition, the [official Intel documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/)
provides a comprehensive reference of all available functions. Note that our the department machines only consistently support
the "SSE" and "AVX" and "AVX2" functions. So, when using the Intel page, only check the boxes labelled "SSE" through "SSE4.2",
"AVX", and "AVX2"

# Example SIMD function {#exampleSIMD}

In this lab, you will be creating optimized versions of several functions that use vector instructions.
To help you, we have an example created for you already:

*  `add_benchmarks.c` -- normal and vectorized version of an "add two arrays" function. Assumes the array sizes
    are multiples of 16

Compile this by running `make`, then run it by running `./add`. (If you get an error about `bad value (skylake) or -mtune= switch` on a department machine, then run `module load gcc-7.1.0` and run `make` again.)
You will see benchmark results for the two versions of this add two arrays function.

One is this function:

    void add_C(long size, unsigned short * a, const unsigned short *b) {
        for (long i = 0; i < size; ++i) {
            a[i] += b[i];
        }
    }

The other is a version that accesses vector instructions through special "intrinsic functions":

    /* vectorized version */
    void add_AVX(long size, unsigned short * a, const unsigned short *b) {
        for (long i = 0; i < size; i += 16) {
            /* load 256 bits from a */
            /* a_part = {a[i], a[i+1], a[i+2], ..., a[i+15]} */
            __m256i a_part = _mm256_loadu_si256((__m256i*) &a[i]);
            /* load 256 bits from b */
            /* b_part = {b[i], b[i+1], b[i+2], ..., b[i+15]} */
            __m256i b_part = _mm256_loadu_si256((__m256i*) &b[i]);
            /* a_part = {a[i] + b[i], a[i+1] + b[i+1], ...,
                         a[i+7] + b[i+15]}
             */
            a_part = _mm256_add_epi16(a_part, b_part);
            _mm256_storeu_si256((__m256i*) &a[i], a_part);
        }
    }

## New Types {#sseTypes}

An `__m256i` represents a 256-bit value that can be stored on one of the special 256-bit `%xmm` registers
on our the department machines. The `i` indicates that the 256-bit value contains an array of integers. In this case,
they are 16-bit integers, but we can also work with other sized integers that fit in 256 bits.

Whenever we want to get or use a `__m256i` value, we will use one of the special functions whose
name begins `_mm256`. You should not try to extract values more directly. (This may compile, but will probably not
do what you expect and may differ between compilers.)

We also have some functions that take a `__m256i*`. This is a pointer to a 256-bit value which can
be loaded into a 256-bit register. When we cast `&a[i]` to this type we are indicating that we mean "256 bits
starting with a[i]". Since each element of `a` is 16 bits, this means `a[i]` up to and including `a[i+15]`.

## New "intrinisc" functions {#sumIntrinsics}

To manipulate the 256-bit values we use several intrinsic functions:

*  `__m256i _mm256_loadu_si256(__m256i* ptr)`: loads 256-bits from in memory from `ptr` .
   In this case, those 256-bits represent a vector of sixteen 16-bit values. For example
   `a_part` represents the vector `{a[i], a[i+1], a[i+3], a[i+4], a[i+5], a[i+6], ...,  a[i+15]}`.

*  `__m256i _mm256_add_epi16(__m256i x, __m256i y)`: treat the 256-bit values as a
    vector of 16-bit values, and add each pair.
    if `x` is the vector `{x[0], x[1], x[2], x[3], x[4], x[5], ..., x[15]}` and
    `y` is the vector `{y[0], y[1], y[2], y[3], y[4], y[5], ..., y[15]}`, then the result is
    `{x[0] + y[0], x[1] + y[1], x[2] + y[2], x[3] + y[3],
      x[4] + y[4], x[5] + y[5], ..., x[15] + y[15]}`.

*  `void _mm256_storeu_si256(__m256i* ptr, __m256i value)`: store 256 bits into memory into `ptr`.

Each of these functions will **always** be inlined, so we do not need to worry about function call overhead.
Most of the special of `_mm256` function will compile into one instruction or a fixed
sequence of two instructions (as you can see below)

The `epi16` part of some function names probably stands for "extended packed 16-bit", indicating that it
manipulates a vector of 16-bit values.

## 256 or 128 bit?

There are also 128-bit versions of most of the 256-bit functions, with the following differences:

*  the function names for the 128-bit versions start with `_mm_` instead of `_mm256_`
*  the type of a 128-bit vector is `__m128i` instead of `__m256i`.

The ISA extensions with the 128-bit versions are called "SSE", while the 256-bit versions are called "AVX".

For example, a version of the add function with 128-bit vectors looks like:

    /* vectorized version */
    void add_SSE(long size, unsigned short * a, const unsigned short *b) {
        for (long i = 0; i < size; i += 8) {
            /* load 128 bits from a */
            /* a_part = {a[i], a[i+1], a[i+2], ..., a[i+7]} */
            __m128i a_part = _mm_loadu_si128((__m128i*) &a[i]);
            /* load 128 bits from b */
            /* b_part = {b[i], b[i+1], b[i+2], ..., b[i+7]} */
            __m128i b_part = _mm_loadu_si128((__m128i*) &b[i]);
            /* a_part = {a[i] + b[i], a[i+1] + b[i+1], ...,
                         a[i+7] + b[i+7]}
             */
            a_part = _mm_add_epi16(a_part, b_part);
            _mm_storeu_si128((__m128i*) &a[i], a_part);
        }
    }

## Intrinsics and assembly

Typical assembly code generated for `add_AVX` above looks like:

    add_AVX:
      // size <= 0 --> return
      testq %rdi, %rdi
      jle end_loop

      // i = 0
      movl $0, %eax

    start_loop:
      // __m256i b_part = _mm256_loadu_si256((__m256i*) &b[i]);
        // compiles into two instructions, each of which loads 128 bits
      vmovdqu (%rdx,%rax,2), %xmm0
      vinserti128 $0x1, 16(%rdx,%rax,2), %ymm0, %ymm0

      // __m256i a_part = _mm256_loadu_si256((__m128i*) &b[i]);
      vmovdqu (%rsx,%rax,2), %xmm1
      vinserti128 $0x1, 16(%rsx,%rax,2), %ymm1, %ymm1

      // a_part = _mm256_add_epi16(a_part, b_part);
      vpaddw %ymm1, %ymm0

      // _mm256_storeu_si256((__m256i*) &a[i], a_part)
      vmovups %ymm0, (%rsi,%rax,2)
      vextracti128 $0x1, %ymm0, 16(%rsi,%rax,2)

      // i += 16
      addq $16, %rax
      
      // i < size --> return
      cmpq %rax, %rdi
      jg start_loop
    end:
      ret

(You can see the actual code in `add_benchmarks.s`.)

(Various details will vary between compilers, and with some optimization settings, compilers might try to
perform other optimizations, like loop unrolling.)

Each of the `_mm256_` functions corresponds directly to one or two assembly instructions:

*  `_mm256_loadu_si256` turns into a `vmovdqu` 
*  `_mm256_add_epi16` turns into `vpaddw`
*  `_mm256_storeu_si256` turns into `vmovups`

# Task 1: Sum with Intel intrisics {#sumTask}

The first coding task is to create a version of `sum`:

    unsigned short sum_C(long size, unsigned short * a) {
        unsigned short sum = 0;
        for (int i = 0; i < size; ++i) {
            sum += a[i];
        }
        return sum;
    }

that uses vector instructions through the intrinsic functions.

Start by making a copy of the provided `sum_with_sixteen_accumulators` that uses 16 accumulators.

Rename this copy `sum_AVX`.

Since the loop performs sixteen independent additions of 16-bit values, it can be changed
to use a single call to `_mm256_add_epi16`:

*  Instead of storing these sixteen 16-bit accumulators in separate variables,
   declare a single  `__m256i` variable (perhaps called `partial_sums`), which will contain all of their values.
   You can initialize it zero with something like:

        __m256i partial_sums = _mm256_setzero_si256();

*  Instead of loading `a[i+0]` through `a[i+15]` seperately, call
    `_mm256_loadu_si256` to load them all into a single `__m256i` variable.
   This may be identical to how `a_part` is set in `add_AVX` above.
*  Instead of performing 16 additions, use one call to `_mm256_add_epi16` with
   `partial_sums` and `a_part` (or whatever you called these variables)
*  After the loop, store the 16 partial sums in a temporary array on the stack
   using `_mm256_storeu_si256`:
        
        unsigned short extracted_partial_sums[16];
        _mm256_storeu_si256((__m256i*) &extracted_partial_sums, partial_sums);

   Then, add up these sixteen partial sums.

When you've completed this `sum_AVX` function, add it to the list of functions in
`sum_benchmarks.c`, then run `make` to compile it. Then compare its performance to
the other versions using `./sum`. Make sure that `./sum` reports that your solution
does not result in an incorrect answer.

Also examine the assembly code the compiler generated for your `sum_benchmarks.c` in
`sum_benchmarks.s`.

(It is also possible to perform the last 16 additions in parallel, without copying to the stack first,
but for simplicitly and because it has a small effect on performance, we will not require that here.)

# Task 2: Vectorized min {#minTask}

The next task is, using the same idea as you used to vectorize the `sum`, create a vectorized version  of this min function:

    short min_C(long size, short * a) {
        short result = SHRT_MAX;
        for (int i = 0; i < size; ++i) {
            if (a[i] < result)
                result = a[i];
        }
        return result;
    }

which you can find in `min_benchmarks.c`. Create a new version of this that acts on `__m256i` variables containing
sixteen elements of the array at a time. Some intrinsic functions that may be helpful (you can also
refer to our [reference page](simdref.html) or the [Intel documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/)):

*  `__m256i _mm256_setr_epi16(short a1, short a2, short a3, short a4, short a5, short a6, short a7, short a8, shot a9, short a10, short a11, short a12, short a13, short a14, short a15, short a16)` ---
    returns a `__m256i` containing representing vector of signed 16-bit values.
   `a1` will be the value that would be stored at the lowest memory address.
   (Avoid this when `_mm256_loadu_si256` would work without rearranging values and the arguments are not constants: it will often be slower. We mention it primarily to explain `set1` clearly.)
*  `_mm256_set1_epi16(short a)` --- same as `_mm_setr_epi16(a, a, a, a, a, a, a, a,  a, a, a, a, a, a, a, a)`.
*  `_mm256_min_epi16(a, b)`. Assumes that `a` and `b` contain a vector of sixteen 16-bit signed integers. Returns the minimums
   of each pair. For example:

        __m256i first =  _mm256_setr_epi16(-0x0100, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, -0x6000, 0x7000,
                                           0, 0, 0, 0, 0, 0, 0, 0);
        __m256i second = _mm256_setr_epi16(0x1000, 0x2000, 0x0100, 0x2000, 0x5000, 0x7FFF, -0x1000, -0x7000,
                                           1, 1, 1, 1, 1, 1, 1, 1);
        __m256i result = _mm256_min_epi16(first, second)

    makes `result` contain `{-0x0100, 0x1000, 0x0100, 0x2000, 0x4000, 0x5000, -0x6000, -0x7000, 0, 0, 0, 0, 0, 0 ,0 ,0}`.
    (`-0x0100` is the minimum of the first elements `-0x0100` and `0x1000`; `0x1000` is the minimum of the second elements `0x1000` and `0x2000`, and so on.)

After adding your vectorized function to `min_benchmarks.c` and *adding it to the list of functions*, test it by running `make` and then `./min`.

# Task 3: Vectorize dot-product {#dotProductTask}

Now let's vectorize the following function:

    unsigned int dot_product_C(long size, unsinged short *a, unsigned short *b) {
        unsigned int sum;
        for (int i = 0; i < size; ++i)
            sum += a[i] * b[i];
        return sum;
    }

Note that this function computes its sums with `unsigned int`s instead of `unsigned short`s,
so you'll need to add 32-bit integers
instead of 16 bit integers. 
So, you will have 256-bit values which contain eight 32-bit integers instead of sixteen 16-bit integers.
To obtain these originally, you'll need to convert the 16-bit integers you read from the array into 32-bit integers; fortunately, there is an vector instruction (and intrinsic function) to do this quickly.
To manipulate these as 32-bit integers, you will use functions containing `epi32` in their names instead `epi16` name, which correspond
to different vector instructions.

Some intrinsic functions which may be helpful:

*  `__m256i _mm256_add_epi32(__m256i x, __m256i y)` is like `_mm_add_epi16` but expects vectors of eight 32-bit integers instead of sixteen 16-bit integers.

*  `__m256i _mm256_setzero_si256()` returns an all-zeroes 256-bit value. When interpreted as a vector of integers of any size, it will be all 0 integers.

*  `__m128i _mm_loadu_si128(__m128i *p)` load 128-bits from address `p` into a ***128-bit*** vector.

*  `__m256i _mm256_cvtepu16_epi32(__m128i x)` if `x` contains a ***128-bit vector*** of eight 16-bit unsigned integers, convert them into a 256-bit vector of 32-bit integers. For example:

        unsigned short foo[8] = {1, 2, 3, 4, 5, 6, 7, 8};
        unsigned int result[8];
        
        __m128i foo_as_vector = _mm_loadu_si128((__m128i*) &foo[i]);
        __m256i foo_converted = _mm256_cvtepu16_epi32(foo_as_vector);
        _mm256_storeu_si256((__m256i*) &result[0], foo_converted);

    makes `result` become `{1, 2, 3, 4, 5, 6, 7, 8}`.

*  `_mm256_mullo_epi32(x, y)` --- like `_mm256_add_epi16` but multiply each pair of 32-bit values to produce a 64-bit value, and truncate each product to 32 bits.

Like you did with `sum`, you can add up partial sums at the end by storing them in a temporary array on the stack.

Since you are adding vectors of eight 32-bit values, your loop will probably act on eight elements at a time (even though, in the other problems, you probably used `_mm256_loadu_si256` to load sixteen at a time).

After adding your vectorized function to `dot_product_benchmarks.c` and *adding it to the list of functions*, test it for correctness by running `make` and then `./dot_product`.

(It's possible that your first vectorized version will be slower than the original because you are not using multiple accumulators. Although the vector instructions can perform more computations per unit time, they tend to have high latency.)

# (optional) Task 4: Optimize the vectorized dot-product {#dotProductOptimize}

Make a copy of your vectorized dot-product function and see how it is affected by applying various possible optimizations.
Things you might try include:

*  loop optimizations from the last lab, such as multiple accumulators.
*  using different vector instructions. For example, based on some [unofficial instruction timing tables](https://www.agner.org/optimize/instruction_tables.pdf), multiplying two 16-bit integers to get a 32-bit integer using `_mm256_mullo_epi16` and `_mm256_mulhi_epi16` together (see [reference links above](#general-simd-reference)) might be faster on some processors than `_mm256_mullo_epi32`.
*  trying to do any final summation using vector instructions.

See if you
can match or beat the performance of the supplied version of `dot_product_C` compiled with GCC 7.2 with  optimization that use vecotr instructions --- or at least try to make it faster than the original plain C version, if it wasn't.
If you are using your labtop, check if the performance difference on your laptop consistent with the the department machines.

# Submission

Run `make simdlab-submit.tar` to create an archive of your C files (called `simdlab-submit.tar`) and upload it
to [kytos](https://kytos.cs.virginia.edu/cs3330/).

Please submit whatever you completed within lab time, even if it is less than the whole lab (so we can at least give partial credit (and possibly full credit if you're close enough to the full lab). Note that you can resubmit later before the deadline if you wish).
