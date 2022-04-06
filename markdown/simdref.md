---
title: "A quick guide to SSE/SIMD"
...

{:.changelog}
Changelog:

{:.changelog}
*  12 November 2020: correct `_mm256_permute2x128_si128` to the (actually existing) `_mm256_permute2x128_si256`

# Introduction

This semester we will  use SIMD (single instruction multiple data)
instructions in several assignments. These are sets of instructions that operate on wide registers
called **vectors**. For our assignments, these vectors will generally be 256 bits wide, though 
you may occassionally use the 128-bit versions. Typically, instructions
that act on these wide registers will treat it as an array of values. They will then perform operations
independently on each value in the array. In hardware, this can be implemented by having multiple
ALUs that work in parallel. As a result, although these instructions perform many times more arithmetic than
"normal" instructions, they can be as fast as the normal instructions.

Generally, we will be accessing these instructions using "intrinsic functions". These functions
usually correspond directly to a particular assembly instruction. This will allow us to write C code
that accesses this special functionality consistently without losing all the benefits of having
a C compiler.

# Intrinsics reference

The intrinsic functions we will be using are an interface defined by Intel. Consequently, 
Intel's documentation, which can be [found here](https://software.intel.com/sites/landingpage/IntrinsicsGuide/)
is the comprehensive reference for these functions. Note that this documentation includes functions
corresponding to instructions which are not supported on lab machines. To avoid seeing these
be sure to check only the boxes labelled "AVX", "AVX2" and "SSE" through "SSE4.2" on the side.

Intel's reference generally describes the instructions in psuedocode that uses notation like
    
    a[63:0] := b[127:64]

to represent assigning bits 64 to 127 (inclusive) of a vector b to bits 0 to 63 of a vector a.

# Header files

To use the intrinsic functions, you need to include the appropriate header file. For the intrinsics we
will be using this is:

    #include <smmintrin.h>
    #include <immintrin.h>

# Representing vectors in C

To represent 256-bit values that might be stored in one of these registers in C, we will use one
of the following types:

*  `__m256` (for eight floats)
*  `__m256d` (for four doubles)
*  `__m256i` (for integers, no matter the size)

Since each of these is just a 256-bit value, you can cast between these types if a function you want
to use expects the "wrong" type of value. For example, you might want to use a function
meant to load floating values to load integers. Internally,
the functions that expect these types just manipulate 256-bit values in registers or memory.

## 128-bit versions of types and intrinsics

There are also 128-bit vector types and corresponding instructions. To use this, for the most
part you can replace `__m256` with `__m128` in the type names and `_mm256_` with
`_mm_` in the type name.

In some cases, only a 256-bit version of an instruction will exist.

## Setting and extracting values

If you want to load a constant in a 128-bit value, you need to use one of the intrinisc functions.
Most easily, you can use one of the functions whose name starts with `_mm_setr`. For example:

    __m256i values = _mm256_setr_epi32(0x1234, 0x2345, 0x3456, 0x4567, 0x5678, 0x6789, 0x789A, 0x89AB);

makes `values` contain 8 32-bit integers, `0x1234`, `0x2345`, `0x3456`, `0x4567`, `0x5678`,
`0x6789`, `0x789A`, `0x89AB`. We can then extract
each of these integers by doing something like:

    int first_value = _mm256_extract_epi32(values, 0);
    // first_value == 0x1234
    int second_value = _mm256_extract_epi32(values, 1);
    // second_value == 0x2345

Note that ***one may only pass constant indices*** to the second argument of
`_mm256_extract_epi32` and similar functions.

## Loading and storing values

To load an array of values from memory or store an array of values to memory, we can use
the intrinsics starting with `_mm256_loadu` or `_mm256_storeu`:

    int arrayA[8];
    _mm256_storeu_si256((__m128i*) arrayA, values);
    // arrayA[0] == 0x1234
    // arrayA[1] == 0x2345
    // ...

    int arrayB[8] = {10, 20, 30, 40, 50, 60, 70, 80};
    values = _mm256_loadu_si256((__m128i*) arrayB);
    // 10 == arrayB[0] == _mm256_extract_epi32(values, 0)
    // 20 == arrayB[1] == _mm256_extract_epi32(values, 1)
    // ...

## Arithmetic

To actually perform arithmetic on values, there are functions for each of the
supported mathematical operations. For example:

    __m256i first_values =  _mm256_setr_epi32(10, 20, 30, 40);
    __m256i second_values = _mm256_setr_epi32( 5,  6,  7,  8);
    __m256i result_values = _mm256_add_epi32(first_values, second_values);
    // _mm_extract_epi32(result_values, 0) == 15
    // _mm_extract_epi32(result_values, 1) == 26
    // _mm_extract_epi32(result_values, 2) == 37
    // _mm_extract_epi32(result_values, 3) == 48


## Different types of values in vectors

The examples treat the 256-bit values as an array of 8 32-bit integers. There are instructions
that treat in many different types of values, including other sized integers or floating
point numbers. You can usually tell which type is expected by the presence of a something
indicating the type of value in the function names. For example, "epi32" represents
"8 32-bit values" in an `__m256` or "4 32-bit values" in an `__m128`
(The name stands for "extended packed integers, 32-bit".) Some
other conventions in names you will see:
 
*  `si256` -- signed 256-bit integer
*  `si128` -- signed 128-bit integer
*  `epi8`, `epi32`, `epi64` --- an vector of signed 8-bit integers (32 in a `__m256` and 16 in a `__m128`) or signed 32-bit integers or signed 64-bit integers 
*  `epu8` --- an vecotr of unsigned 8-bit integers (when there is a difference between what an operation would do with signed and unsigned numbers, such as with conversion to a larger integer or multiplication)
*  `epu16`, `epu32` --- an array of unsigned 16-bit integers or 8 unsigned 32-bit integers (when the operation would be different than signed)
*  `ps` --- "packed single" --- 8 single-precision floats
*  `pd` --- "packed double" --- 4 doubles
*  `ss` --- one float (only 32-bits of a 256-bit or 128-bit value are used)
*  `sd` --- one double (only 64-bits of a 256-bit or 256-bit value are used)

# Example (in C)

The following two C functions are equivalent
    
    int add_no_AVX(int size, int *first_array, int *second_array) {
        for (int i = 0; i < size; ++i) {
            first_array[i] += second_array[i];
        }
    }

    int add_AVX(int size, int *first_array, int *second_array) {
        int i = 0;
        for (; i + 8 <= size; ++i) {
            // load 128-bit chunks of each array
            __m256i first_values = _mm_loadu_si256((__m128i*) &first_array[i]);
            __m256i second_values = _mm_loadu_si256((__m128i*) &second_array[i]);

            // add each pair of 32-bit integers in the 128-bit chunks
            first_values = _mm_add_epi32(first_values, second_values);
            
            // store 128-bit chunk to first array
            _mm_storeu_si128((__m128i*) &first_array[i], first_values);
        }
        // handle left-over
        for (; i < size; ++i) {
            first_array[i] += second_array[i];
        }
    }


# Selected handy intrinsic functions:

## Arithmetic
*  `_mm256_add_epi32(a, b)` --- treats its `__m256i` arguments as 8 32-bit integers. If `a` contains the 32-bit integers `a0, a1, a2, a3, a4, a5, a6, a7` and `b` contains
    `b0, b1, b2, b3, b4, b5, b6, b7`, returns `a0 + b0, a1 + b1, a2 + b2, a3 + b3, a4 + b4, a5 + b5, a6 + a6, a7 + b7`. (Corresponds to the `vpaddd` instruction.)
*  `_mm256_add_epi16(a, b)` --- Same as `_mm256_add_epi32` but with 16-bit integers. If `a` contains the 16-bit integers `a0, a1, ..., a15` and `b` contains
    `b1, b2, ..., b15`, returns `a0 + b0, a1 + b1, ..., a15 + b15`. (Corresponds to the `vpaddw` instruction.)
*  `_mm256_add_epi8(a, b)` --- Same as `_mm256_add_epi32` but with 8-bit integers.
*  `_mm256_mullo_epi16(x, y)`: treats x and y as a vector of 16-bit signed integers, multiplies each pair of integers, and truncates the results to 16 bits.
*  `_mm256_mulhi_epi16(x, y)`: treats x and y as a vector of 16-bit signed integers, multiplies each pair of integers to get a 32-bit integer, then returns the top 16 bits of each 32-bit integer result.
*  `_mm256_srli_epi16(x, N)`: treat `x` and a vector of 16-bit signed integers, and return the result of logically shifting each right by `N`. (There is also a `epi32` and `epi64` variant for 32 or 64-bit integers.)
*  `_mm256_slli_epi16(x, N)`: treat `x` and a vector of 16-bit signed integers, and return the result of shifting each left by `N`. (There is also a `epi32` and `epi64` variant for 32 or 64-bit integers.)
*  `_mm256_hadd_epi16(a, b)` --- ("horizontal  add") treats its `__m128i` arguments as vectors of 16-bit integers. If `a` contains `a0, a1, a2, a3, ..., a15` and `b` contains `b0, b1, b2, b3, ..., b15`, returns `a0 + a1, a2 + a3, a4 + a5, a6 + a7, b0 + b1, b2 + b3, b4 + b5, b6 + b7, a8 + a9, a10 + a11, a12 + a13, a14 + a15, b8 + b9, b10 + b11, b12 + b13, b14 + b15`. Note that this is often substantially slower than `_mm_add_epi16`. (Corresponds to the `vphaddw` instruction.) 

## Load/Store
*  `_mm256_loadu_si256`, `_mm256_storeu_si256` --- load or store 256 bits to or from memory. Note that you can use `_mm256_storeu_si256` to store into a temporary array as in:

        unsigned short values_as_array[16];
        __m256i values_as_vector;

        _mm256_storeu_si128((__m256i*) &values_as_array[0], values_as_vector);

*  `_mm_loadu_si128`, `_mm_storeu_si128` --- load or store 128 bits to or from memory. (Corresponds to the `vmovdqu` instruciton.) They work exactly like the `_mm256_loadu_si256` except that they use the type `__m128i` instead of `__m256i`.

*  To store 64 or 32 bits from a vector, one way is to use an extract operation and memcpy:

        unsigned short first_four_values_as_array[4];
        __m256i values_as_vector;

        *(long*)(&first_four_values_as_array[0]) = _mm256_extract_epi64(values_as_vetor, 0);

    (This code is not actually standard complaint; it violates "strict aliasing" rules. But in the Makefile for the SIMD
    assignments, we've disabled this with the compiler option `-fno-strict-aliasing`. An alternative that wouldn't violate 
    strict aliasing rules would be to use a union instead of casting a pointer to an `int*` or to use `memcpy`, which is
    typically optimized away for small copies.)

*  `_mm_cvtsi32_si128`: load 32 bits into a 128-bit vector:
    
        unsigned short values[2];
        __m128i values_as_vector; // only using first 32 bits = 2 shorts
        values_as_vector = _mm_cvtsi32_si128( *(int*) &values[0]);

    (This code is not actually standard complaint; it violates "strict aliasing" rules. But in the Makefile for the SIMD
    assignments, we've disabled this with the compiler option `-fno-strict-aliasing`. An alternative that wouldn't violate 
    strict aliasing rules would be to use a union instead of casting a pointer to an `int*`.)

*  `_mm_cvtsi32_si128`: load 64 bits into a 128-bit vector:

        unsigned short values[4];
        __m128i values_as_vector; // only using first 64 bits = 4 shorts
        values_as_vector = _mm_cvtsi64_si128( *(long*) &values[0]);
    
    (This code is not actually standard complaint; see comment above for `_mm_cvtsi32_si128`)

*  To load 32 or 64 bits in a 256-bit vector, you can use `_mm_cvtsi32_si128` or `_mm_cvtsi32_si256` together with `_mm266_zextsi128_si256` to convert a 128-bit vector to a 256-bit one.

*  `_mm256_maskstore_epi32(int *addr, __m256i mask, __m256i a)` --- store 32-bit values from `a` at `addr`, but only the values
    32-bit values that `mask` specifies. Values are stored if the most significant (i.e. sign) bit of each 32-bit integer in `mask`
    is set. For example:

        int values[8] = { 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF };
        __m256i a =    __m256_setr_epi32(1,2,3,4,5,6,7,8);
        __m256i mask = __m256_setr_epi32(0,-1,0,0,-1,0,-1,-1);
        _mm256_maskstore_epi32(&values[0], mask, a);

    should result in values containing
        
        { 0xF, 2, 0xF, 0xF, 5, 0xF, 7, 8 }

*  For more see the [Intel's reference, under the Load and Store categories](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#cats=Load,Store&techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX,AVX2)

<!-- FIXME: multiple/subtract/min/etc. -->
## Set constants

*  `_mm256_setr_epi32` --- returns a `__m256i` value containing the specified 32-bit integers. The first integer argument will be in the part of the `__m256i` that has the lowest address when written to memory. For example: 

        __m256i value1 = _mm256_setr_epi16(0, 1, 2, 3, 4, 5, 6, 7);

    produces the same result in `value1` as in `value2` in

        int array[8] = {0, 1, 2, 3, 4, 5, 6, 7};
        __m256i value2 = _mm256_loadu_si256((__m256i*) &array[0]);

*  `_mm_setr_epi32` --- returns a `__m128i` value containing the specified 32-bit integers. The first integer argument will be in the part of the `__m128i` that has the lowest address when written to memory. For example: 

        __m128i value1 = _mm_setr_epi32(0, 1, 2, 3);

    produces the same result in `value1` as in `value2` in

        int array[4] = {0, 1, 2, 3, 4, 5, 6, 7};
        __m128i value2 = _mm_loadu_si128((__m256i*) &array[0]);


*  `_mm256_setr_epi16` --- same as `_mm256_setr_epi32` but with 16-bit integers. For example:

        __m256i value1 = _mm256_setr_epi16(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    produces the same result in `value1` as in `value2` in

        short array[8] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        __m256i value2 = _mm256_loadu_si256((__m256i*) &array[0]);

*  `_mm256_setr_epi8`, `_mm_setr_epi8` --- same as `_mm256_setr_epi32` and `_mm_setr_epi32` but with 8-bit integers.

*  `_mm_set1_epi32`, `_mm_set1_epi16`, `_mm_set1_epi8` --- return a `__m128i` value representing an array of values of the appropriate size, where
    each element of the array has the same value. For example:

        __m128i value = _mm_set1_epi16(42);

    has the same effect as:

        __m128i value = _mm_setr_epi16(42, 42, 42, 42,  42, 42, 42, 42);

*  `_mm256_set_epi8`, etc.  --- same as `_mm256_setr_epi8`, etc. but takes its arguments in reverse order

*  For more see the [Intel's reference, under the Set category](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#cats=Set&techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX,AVX2)

## Extract parts of values 

*  `_mm256_extract_epi32(a, index)` extracts the `index`'th 32-bit integer from the 256-bit vector `a`. The integer with index 0 is the one that will be stored at the lowest memory address if `a` is copied to memory. `index` ***must*** be a constant.
    
    For example
        
        __m256i a = _mm256_setr_epi32(0, 10, 20, 30, 40, 50, 60, 70);
        int x = _mm256_extract_epi32(a, 2);

    assigns `20` to `x`.

  
*  `_mm_extract_epi32(a, index)` extracts the `index`'th 32-bit integer from the 128-bit vector `a`. `index` must be constant.

*  `_mm256_extract_epi16(a, index)` is same as `_mm256_extract_epi32` but with 16-bit integers

*  `_mm256_extracti128_si256(a, index)` extract the `index` 128-bit vector from the 256-bit vector `a`. `index` must be constant.

    For example

        __m256i a = _mm256_setr_epi32(0, 10, 20, 30, 40, 50, 60, 70);
        __m128i result = _mm256_extracti128_si256(a, 1);

    is equivalent to
        
        __m128i result = _mm_setr_epi32(40, 50, 60, 70);


*  For more see the [Intel's reference](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX,AVX2), searching for "extract" or looking under the "Swizzle" and "Cast" categories.

## Convert between types of values

*  `_mm256_cvtepu8_epi16(eight_bit_numbers)`: takes a 128-bit vector of sixteen 8-bit numbers, and converts it to a 256-bit vector of sixteen 16-bit signed integers. For example:

        __m128i value1 = _mm_setr_epi8(10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150);
        __m256i value2 = _mm256_cvtepu8_epi16(value1);

    results in value2 containing the same value as if we did:

        __m256i value2 = _mm256_setr_epi16(10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150);

*  <a name="packus"></a>`_mm256_packus_epi16(a, b)` takes the 16-bit signed integers in the 256-bit vectors `a` and `b` and converts
    them to a 256-bit vector of 8-bit unsigned integers.
    The result contains the first 8 integers from `a`, followed by the first 8 integers from `b`, followed by
    the last 8 integers from `a`, followed by the last 8 integers from `b`.
    Values that are out of range are set to 255 or 0.

    For example:

        __m256i a = _mm256_setr_epi16(10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160);
        __m256i b = _mm256_setr_epi16(170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 25, 15, 5, -5, -15);

        __m256i result = _mm256_packus_epi16(a, b)

    sets `result` the same as if we did:
        
        __m256i result = _mm256_setr_epu8(
            10, 20, 30, 40, 50, 60, 70, 80, /* first 8 integers from a */
            170, 180, 190, 200, 210, 220, 230, 240, /* first eight integers from b */
            90, 100, 110, 120, 130, 140, 150, /* last 8 integers from a */
            250, 255, 255, 25, 15, 5, 0, 0,  /* last 8 integers from b */
                /* 260, 270 became 255;  -5, -15 became 0 */
        );
*  `_mm256_zextsi128_si256(a)` takes a 128-bit vector `a` and converts it to a 256-bit vector by adding 0s.

*  For more see the [Intel's reference](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#cats=Swizzle,Move,Cast&techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX,AVX2) under the "Swizzle" and "Move" and "Cast" categories.

## Rearrange 256-bit values

*  `_mm256_permute2x128_si256(a, b, mask)` takes two 256-bit vectors `a` and `b`  and combines the
    128-bit halves of these vectors into a new 256-bit vector according to `mask`.
    `mask` is a one-byte integer constant. The least significant nibble specifies the value placed into the lowest
    address of the resulting vector, the most significant nibble specifies the value placed into the highest address
    of the resulting vector.
    
    The value chosen by each nibble of the mask is:
    *  0 to select the first 128 bits of `a`
    *  1 to select the second 128 bits of `a`
    *  2 to select the first 128 bits of `b`
    *  3 to select the second 128 bits of `b`
    *  4 through 15 to select the constant `0` (ignoring the values of `a` and `b`)

    For example, to repeat the second 128-bits of `a`, one would supply a mask of `0x11` like the following
    example:
        
        __m256i a = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
        __m256i b = _mm256_setr_epi32(8, 9, 10, 11, 12, 13, 14, 15);
        __m256i result = _mm256_permute2x128_si256(a, b, 0x11);
        // result == _mm256_setr_epi32(4, 5, 6, 7, 4, 5, 6, 7)

    To produce a result with the first 128-bits of `a` followed by the second 128-bits of `b` one
    would supply a mask like `0x30`:
        
        __m256i a = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
        __m256i b = _mm256_setr_epi32(8, 9, 10, 11, 12, 13, 14, 15);
        __m256i result = _mm256_permute2x128_si256(a, b, 0x30);
        // result == _mm256_setr_epi32(0, 1, 2, 3, 12, 13, 14, 15)

*  `_mm256_unpackhi_epi16(a, b)` interleaves the 16-bit integers from the top quarter of each 128-bit half of
    the 256-bit vectors `a` and `b`. For example:

        __m256i a = _mm256_setr_epi16(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        __m256i b = _mm256_setr_epi16(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
        __m256i result = _mm256_unpackhi_epi16(a, b);

    is the same as 

        __m256i result = _mm256_setr_epi16(
            /* top quarter of first half of a and b */
            4, 20, 5, 21, 6, 22, 7, 23,
            /* top quarter of second half of a and b */
            12, 28, 13, 29, 14, 30, 15, 31
        )

*  `_mm256_unpacklo_epi16(a, b)` is like `_mm256_unpackhi_epi16` but it takes the 16-bit integers from
    the bottom quarter of each half of `a` and `b`

*  `_mm256_permutevar8x32_epi32(x, indexes)` --- Produce a vector of 32-bit values by, for each 32-bit index in the vector `indexes`, 
    retrieving the 32-bit value at that index from the vector `x` and place it in the result. For example:

        __m256i x = _mm256_setr_epi32(10, 20, 30, 40, 50, 60, 70, 80)
        __m256i indexes = _mm256_setr_epi32(3, 3, 0, 1, 2, 3, 6, 7);
        __m256i result = _mm256_permutevar8x32_epi32(x, indexes)

    is the same as:

        __m256i reuslt = _mm256_setr_epi32(40, 40, 10, 20, 30, 70, 80);


*  For more see the [Intel's reference](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#cats=Swizzle,Move,Cast&techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX,AVX2) under the "Swizzle" and "Move" and "Cast" and "Shift" categories.


## Rearrange 128-bit values

*  `_mm_unpackhi_epi16(a, b)` interleaves the 16-bit integers from the top half of the 128-bit vectors
    `a` and `b`. For example:

        __m128i a = _mm_setr_epi16(0, 1, 2, 3, 4, 5, 6, 7);
        __m128i b = _mm_setr_epi16(8, 9, 10, 11, 12, 13, 14, 15);
        __m256i result = _mm_unpackhi_epi16(a, b);

    is the same as 

        __m128i result = _mm_setr_epi16(
            4, 20, 5, 21, 6, 22, 7, 23,
        )

*  `_mm_shuffle_epi8(a, mask)` rearrange the bytes of `a` according to `mask` and return the result. `mask` is a vector of 8-bit integers (type `__m128i`) that indicates how to rearrange each byte:
    * if a byte in the mask has the high bit set (is greater than 127), then the corresponding byte of the output is 0;
    * otherwise, the byte number specified in the input is copied to the corresponding byte of the output. Bytes are numbered using 0 to represent the byte that would be stored in the lowest address if the vector were copied to memory.

    For example:

        __m128i value1 = _mm_setr_epi8(10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160);
        __m128i mask = _mm_setr_epi8(0x80, 0x80, 0x80, 5, 4, 3, 0x80, 7, 6, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
        __m128i value2 = _mm_shuffle_epi8(value1, mask);

    should produce the same result as:

        __m128i value2 = _mm_setr_epi8(0, 0, 0, 60, 50, 40, 0, 80, 70, 0, 0, 0, 0, 0, 0, 0, 0);
            /* e.g. since 3rd element of mask is 5, 3rd element of output is 60, element 5 of the input */


*  For more see the [Intel's reference](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#cats=Swizzle,Move,Cast&techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2) under the "Swizzle" and "Move" and "Cast" and "Shift" categories.

# Example (assembly instruction)

The instruction

```asm
    paddd %xmm0, %xmm1
```

takes in two 128-bit values, one in the register `%xmm0`, and another in the register `%xmm1`. Each
of these registers are treated as an array of two 64-bit values. Each pair of 64-bit values is added
together, and the results are stored in `%xmm1`.

For example, if `%xmm0` contains the 128-bit value (written in hexadecimal):

    0x0000 0000 0000 0001 FFFF FFFF FFFF FFFF 

and `%xmm1` contains the 128-bit value (written in hexadecimal):

    0xFFFF FFFF FFFF FFFE 0000 0000 0000 0003 

Then `%xmm0` would be treated as containing the numbers `1` and `-1` (or `0xFFFFFFFFFFFFFFFF`), and
`%xmm1` as containing the numbers `-2` and `3`. `paddd` would add `1` and -2` to produce -1 and
`-1` and `3` to produce 2, so the final value of `%xmm1` would be:

    0xFFFF FFFF FFFF FFFF 0000 0000 0000 0002

If we interpret this value as an array of two 64-bit integers, then that would be  `-1` and `2`.

