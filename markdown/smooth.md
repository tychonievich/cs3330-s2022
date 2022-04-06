---
title: "Smooth"
...

# Task

1.  Download [smooth.tar](files/smooth.tar) on a Linux system.

2.  Run `tar xvf smooth.tar` to extract the tarball, creating the directory `smooth`.

3.  Edit `smooth.c` to make the `who_t` structure to include the your name and computing ID.
    You also need to supply a scoreboard name.
    We will publicly post the *last* performance results on a [scoreboard](smoothboard.html),
    which will use whatever scoreboard name you supply. (There will also be a 
    [more detailed result viewer](https://kytos.cs.virginia.edu/cs3330/smoothviewer)
    only visible to you. This will update for checkpoint submissions, but the estimated score
    the result viewer shows is the estimated score for the final (not checkpoint) submission.)
    
4.  In `smooth.c`, create new implementations of `smooth` that perform the same calculation
    as `naive_smooth`, but will be faster. Modify `register_smooth_functions` to include
    each of these functions.

    We provide [very detailed hints](#hints) about how to do this below.

    We have deliberately disabled the compiler optimizer's use of vector instructions.
    So, to achieve the desired speedup for the final submission,
    you will probably need to use vector instrinsics, and
    may wish to refer to this [reference](simdref.html) in addition to the hints below.

    For the checkpoint submission, you should not need to use vector instructions
    to achieve the desired speedup, but you are welcome to do so.

    You **may not** attempt to interfere with the code that will time your new implementations
    of `smooth`. (See [other rules](#other rules) below.)

    Your functions only need to work for multiple-of-32 dimension values.
    
5.  Test your implementation to make sure it is correct and to see what average speedup it gets.
    
    1.  You can run `make` to build `./benchmark` to test your smooth implementation on
        your own machine or on a department machine. Usually, if version A of smooth is
        significantly faster on your machine, it will also be faster on ours, but
        the amount faster may vary significantly.

        You should always do this before submitting something for our
        server to run to make sure your new version of smooth performs the correct calculation.

    2.  If you submit `smooth.c` to [kytos](https://kytos.cs.virginia.edu/cs3330/){.target="_top"}, we
        will time it on our machine and post the results
        [here](https://kytos.cs.virginia.edu/cs3330/smoothviewer/){.target="_top"}.
        It may take 30-60 miuntes for us to run your submission, and perhaps more if we receive
        a lot of submissions in a short amount of time.

6.  For the checkpoint submission, you must get a speedup of at least 2.50 ***on our testing machine***.

    To get full credit for the final submission, your speedup should be at least 4.75 ***on our testing machine***. (See [grading](#grading) below for
    more details.)

    There is a seperate submission form for the checkpoint and final submission on kytos. We will compute
    the grade of all your submissions as if they were submitted to
    both the checkpoint and final submission (taking into account late penalties),
    and only keep the highest scores (even if you submit something that scores lower later)
    when computing your grade.


# Collobaration Policy

The following kinds of conversations are permitted with people other than the course staff:

*	drawings and descriptions of how you want to handle memory accesses across an image

*	code snippets explaining optimizations, but only for code unlike the smooth and rotate problems we are discussing

*	discussion of the code we provide you: how `RIDX` works, what the naive implementations are doing, etc.

# Other rules

Violations of the following rules will be seen as cheating, subject to penalties that extend beyond the scope of this assignment:

*	You must not modify or interfere with the timing code 
*	You must not attempt to inspect or modify the network, disk, or other grading platform resources

Violations of the following rules will be seen as unprofessional conduct, subject to penalties that extend beyond the scope of this assignment:

*	You must not use a scoreboard name that is offensive or insulting

Additionally, the following rules may result in grade penalties within this assignment if violated:

*	You must write valid C code. (But you may use GCC-specific extensions, except those that would make the compiler reenable vectorization optimizations that our Makefile disables.)
*	You should not turn in code that contains print statements.
*	Your code must work (i.e., the same functionality as the provided naive implementations) for any image of any multiple-of-32 dimension (32, 64, 96, etc).

# Hints {#hints}

In order the achieve the required speedup, you will almost certainly need to use AVX intrinsics to implement smooth.
With the compiler setings we are using, I have gotten speedups of around 2.9x without the AVX intrinsics, but our target is 4.75x.
We have provided a [guide to AVX intrinsics](simdref.html). For the rotate part of the assignment, we will not provide much guidance
about what optimization techniques you must use. But, since most of the work in the smooth part of the assignment will
be figuring out how to effectively use the AVX intriniscs, we will provide pretty detailed guideance on an effective strategy to use.
This is not the only strategy you can use, and you can definitely get more speed by applying additional optimizations.

(With more aggressive compiler settings than we have chosen, our compiler will *sometimes* generate AVX instructions and *sometimes* not, depending on exactly how the code is written. In this assignment, we are trying to give practice that would be useful in the relatively common scenarios where the compiler cannot figure out how to vectorize the function for some reason, and give you an idea what the compiler is doing when it vectorizes the loop.) 

## Strategy
There are some steps we recommend taking when implementing smooth, with steps 1 and 2 (and possibly 5) being what we recommend doing for the checkpoint submission:

1.  First, separate out the special cases of the edges and corners from the loop over each pixel.
2.  Then, focus on the loop over the pixels in the middle of the image. Fully unroll the nested loop over the 3x3 square of pixels.
3.  Then, convert that fully unrolled loop to use vector instructions to add whole pixels (arrays of 4 values) at a time rather than adding one value at a time. Make sure the vector instructions use multiple accumulators (the processor can start multiple vector-adds at a time).
4.  Then, do one or more of the following:
    *  A. Convert your loop over each pixel to compute values for four pixels in a single vector register, or
    *  B. Perform the division by 9 and conversion from 16-bit intermediate values to 8-bit intermediate values using the AVX intrinsics, or 
5.  If things still aren't fast enough, try unrolling the loop over each destination pixel (more). When doing this, in each iteration, it may be helpful to load values from the source image before storing any values to the destination image so the compiler's optimizer is not hampered by aliasing when trying to do optimizations like common subexpression elimination.

On our test machine with our reference implementations:

*  performing steps 1 gets about a 1.6x speedup;
*  performing steps 1-2 gets about a 2.7x speedup;
*  performing steps 1-3 gets about a 2.9x speedup;
*  performing steps 1-3, and step 5 gets about a 4.5x speedup;
*  performing steps 1-3, and option A from step 4 gets about a 4.5x speedup;
*  performing steps 1-3, and option A from step 4 and step 5 gets about a 5.9x speedup;
*  performing steps 1-3, and option B from step 4 gets about a 4.7x speedup;
*  performing steps 1-3, and option B from step 4 and step 5 gets about a 5.9x speedup;
*  performing steps 1-3, and both option A and B from step 4 gets about an 11x speedup

There may be other ways, perhaps more efficient, of vectorizing and optimizing the computation.
Your grade will be based on the speedup you get, not whether you follow our recommendations.

## Separating out the special cases

The main loop of smooth has three different cases:

*  The destination pixel is in the middle of the image, so we load 9 pixels, add them together, then divide by 9.
*  The destination pixel is on a corner of the image, so we load 4 pixels (from varying sides of the destination pixel), add them together, then divide by 4.
*  The destination pixel is on the edge of the image, so we load 6 pixels (from varying sides of the destination pixel), add them together, then divide by 6.

The provided code figures out which of these 3 cases we are in **for every pixel**. Since the calculations we perform for each pixel are quite simple,
this check is costly.
Instead, you can write a loop over the middle of the image, which always loads 9 pixels, and separate loops for
the corners and edges. When you do this, you'll avoid performing comparisons for most of the pixels that the current code performs. 

When you've created a separate loop for the middle of the image, remove redundant computations from it:

*  the inner loop doesn't need to check whether i + 1, i - 1, etc. is beyond the bounds of the image
*  the inner loop can hard-code the number `9` to divide by rather than counting how many pixels are added

When you do so, it would make sense to inline the code (meaning copy from the function into where you
would call it instead of calling it) from `avg`
or creating a separate version of the `avg` function.
*We recommend inlining the code*
so that the compiler is more likely to perform optimizations that avoid recomputing
values (like `i*dim`) between iterations of the loop over the middle of the image.

### A note on division

An additional problem removing special cases in the loop solves is that we compute the number to divide by **for every pixel**.
This means that the compiler usually
needs to generate a divide instruction to perform the division. When the compiler knows that it is performing a division by a constant, it can
do something much faster.
    
For example, it turns out that 1/9 is approximately equal to `7282`/`0x10000`. This means that the compiler can compile

    x / 9

into

    (x * 7282) >> 16

as long as it knows `x` is small enough (and unsigned). Even though this performs many more instructions than using a division instruction, the division
instruction would take many 10s of cycles, and so is considerably slower than performing the calculation this way. But this optimization is only
possible if one can precompute (in this case) `7282` and `16`.

## Unroll the inner-most loops

At this point, you probably have a set of loops over a 3x3 square of pixels.
Unroll these loops completely (so there are nine copies of the loop body and no code that compare a loop index to the loop limit).

## Add pixels with vector instructions.

For this part and anything else with vector instructions, refer to [our general SIMD reference](simdref.html) and our [SIMD lab](simdlab.html):

### Loading pixels into vector registers

We need to load the pixels into vector registers.
In order to perform our calculations accurately, we need to convert 8-bit unsigned chars into 16-bit unsigned shorts in order
to avoid overflow.
To start out, I suggest loading each of the 9 pixels as four 16-bit integers into **its own** `__m256i` value.
Since a `__m256i` can hold sixteen 16-bit integers, this means that you'll ignore the values in three-quarters of the `__m256i`s
we work with for now. This means that when you, for example, use a vector add operation, that it'll add 16 values, but you'll
only end up using 4 of them. Later on, you can work on making more effective use of the vector values.

In order to load the 8-bit values from the `pixel` into a vector of 16-bit values with the AVX intriniscs, I recommend
first loading 128-bits of memory into an `__m128i` (128-bit vector), then using an intrinsic that converts
a 128-bit vector of 8-bit integers to a 256-bit vector of 16-bit integers. (There are also ways to only load one pixel,
but loading 128-bits now will make future steps easier.)

Loading 128 bits will load 4 pixels, though, as mentioned earlier, for now, we'll only use one. For example:

    // load 128 bits (4 pixels)
    __m128i the_pixel = _mm_loadu_si128((__m128i*) &src[RIDX(i, j, dim)]);

will make `the_pixel` contain, when interpreted as **8-bit** integers:

    {
      src[RIDX(i, j, dim)].red,
      src[RIDX(i, j, dim)].green,
      src[RIDX(i, j, dim)].blue,
      src[RIDX(i, j, dim)].alpha,
      src[RIDX(i, j+1, dim)].red,
      src[RIDX(i, j+1, dim)].green,
      src[RIDX(i, j+1, dim)].blue,
      src[RIDX(i, j+1, dim)].alpha
      src[RIDX(i, j+2, dim)].red,
      src[RIDX(i, j+2, dim)].green,
      src[RIDX(i, j+2, dim)].blue,
      src[RIDX(i, j+2, dim)].alpha
      src[RIDX(i, j+3, dim)].red,
      src[RIDX(i, j+3, dim)].green,
      src[RIDX(i, j+3, dim)].blue,
      src[RIDX(i, j+3, dim)].alpha
    }

For now, we will simply ignore the last 12 values we loaded. (Also, you do not need to worry about exceeding the bounds of the source
array; going past the end of the array by 12 bytes should never segfault.)

After loading 8-bit values into this 128-bit vector, you can convert them 16-bit values using
the `_mm256_cvtepu8_epi16` intrinsic function, which will take the 128-bit vector
of 8-bit values and construct a vector of 256-bit vector of 16-bit values. This will ensure that
when you add 9 pixel values together, that overflow does not occur.

### Adding pixels all at once

After loading each of the pixels into a `__m256i`, you can add the pixels using `_mm256_add_epi16`.
To do this most efficiently, recall our discussion of **reassocaition and multiple accumulators**.

After you've added the pixel values together into a vector of 16-bit values extract the values by storing to a temporary array on the stack:

    __m256i sum_of_pixels = ...;

    unsigned short pixel_elements[16];
    _mm256_storeu_si256((__m256i*) pixel_elements, sum_of_pixels);
    // pixel_elements[0] is the red component
    // pixel_elements[1] is the green component
    // pixel_elements[2] is the blue component
    // pixel_elements[3] is the alpha component
    // pixel_elements[4] through pixel_elements[15] are extra values we had stored

Use the values you extracted to perform the division and store the result in the destination array. Below, we have hints
if you want to perform the division and final storing of the result using vector instructions instead of one pixel component at a time.

## Working on groups of four destination pixels

In the previous part, you would have added 16 pairs of 16-bit values together, even though you only used 4 of the 16 16-bit results.

To fix this, let's consider computing several adjacent destination pixels at once. In particular if part of our source image looks like this:

    A B C D E F
    G H I J K L
    M N O P Q R

we could compute all of the following **at the same time**:
*  ***A + B*** + C + G + H + I + M + N + O (destination pixel H) 
*  ***B + C*** + D + H + I + J + N + O + P (destination pixel I)
*  ***C + D*** + E + I + J + K + O + P + Q (destination pixel J)
*  ***D + E*** + F + J + K + L + P + Q + R (destination pixel K)

To illustrate, I'm going to describe how to do the part of the computation in ***bold italics*** above:

Observe that when we load 16 values from the source image, we get four pixels. In particular, since the pixels A through E are adjacent in memory, then, by loading 128-bits from A and converting from 8 to 16-bit integers, we get a `__m256i` containing

    {A.red, A.green, A.blue, A.alpha, B.red, B.green, B.blue, B.alpha, C.red, C.green, C.blue, C.alpha, D.red, D.green, D.blue, D.alpha}

and by loading from B, we get a `__m256i` containing

    {B.red, B.green, B.blue, B.alpha, C.red, C.green, C.blue, C.alpha, D.red, D.green, D.blue, D.alpha, E.red, E.green, E.blue, E.alpha} 

If we add these together using `_mm256_add_epi16`, we get an `__m256i` containing the result of `A+B` and `B+C` and `C+D` and `D+E`:
`{A.red + B.red, A.green + B.green, ..., B.red + C.red, B.green + C.green, ...,  C.red + D.red, C.green + D.green, ..., D.red + E.red, D.green + E.green, ...}`

This is hilited part of both the computation for destination pixels H, I, J, and K, so we can take advantage of this to help compute all of themin parallel.

By taking advantage of this, you can compute the sums for four pixels in parallel.

Near the edges of the image, it may be the case that you don't have a group of four pixels, but only two to compute. In this case, you can use the same code and still compute values for four pixels, but ignore the last two of them when it comes time to write them to the destination image. For this case, this means you will need to use something other than `_mm256_storeu_si256` to write the pixels to `dst`.

For this problem, it happens that loading 128 bits from memory got us four pixels we wanted to work on parallel. If that wasn't the case, there are
instructions to rearrange the bytes in the `%ymm` and `%xmm` registers. 
We describe some of these in [our SIMD quick reference](simdref.html), and you can find a much more complete listing [in Intel's reference](https://software.intel.com/sites/landingpage/IntrinsicsGuide/).
(If you didn't have these instructions, you could still rearrange the pixels using ordinary, non-vectorized code.) Taking advantage of these instructions might also let us find more efficient ways of organizing the computation, for example, not loading pixel values multiple times.

## Performing the division with AVX instructions

As noted above, dividing a 16-bit number `x` by 9 can be performed using the calculation:

    (x * 7282) >> 16

Perform this calculation using the vector instructions. You may wish to refer to [our reference](simdref.html) of 
some additional vector intrinsic functions.

Hint: Although this is the formula for dividing a 16-bit number by 9, you need more than the lower 16 bits of
the result of the multiplication --- otherwise shifting by 16 will always yield 0 (or -1) rather than something useful.
You can, however, avoid doing the entire calculation using 32-bit numbers by taking advantage of `_mm256_mulhi_epi16`.

Then, you need convert 256-bit vector back from 16-bit to 8-bit numbers. Although you could do this by writing it an array
and copying element-by-element from the array into pixels, this is very slow. So, instead,
you can use the `_mm256_packus_epi16` instrinsic
to do this, which we have [an example of in our reference](simdref.html#packus). This takes 16-bit integers 
from two 256-bit vectors and returns a single 256-bit vector of 8-bit integers.

Unfortunately, when working with vectors containing multiple pixels, you may find that 
the `_mm256_packus_epi16` instrinsic function places the 8-bit numbers in an inconvenient order. 
To deal with this, I recommend using `_mm256_unpackhi_epi16`
and/or `_mm256_permute2x128_si256` in order to arrange the arguments to `_mm256_packus_epi16` so that the output 8-bit numbers
are in the order that you want. 

If you need only 128-bits (four pixels) of output,
one way to deal with this would be to compute a 256-bit vector and then extract 128 bits of it using something `_mm256_extracti128_si256`,
then use the 128-bit store function `_mm_storeu_si128` to write the result.

Near the edges of the image or if you are working on less than four pixels at a time otherwise,
you may want to write less than four pixels. To do this, you could
use intrinsics like `_mm256_maskstore_epi32` (example in our SIMD reference) or extract each pixel with
`_mm256_extract_epi32` and store it into the pixel* by casting a pixel* pointer to a int* pointer.


## Extra optimizations

In addition to the above optimization, there are ways that can probably get more speedup:

*  Perform additional loop optimizations, such as more loop unrolling or use of multiple accumulators.
*  Rearranging the vectorized code to avoid loading the same pixels multiple times or adding the same pairs of pixels multiple times
*  Cache optimizations (maybe?)
*  More we haven't thought to mention.


# Code

<b>This section is essentially the same as [rotate's section](rotate.html#Code).</b>

## Structures we give you

A pixel is defined in `defs.h` as

	typedef struct {
	    unsigned char red;
	    unsigned char green;
	    unsigned char blue;
            unsigned char alpha;
	} pixel;

(The "alpha" component represents transparency.)

In memory, this takes up 4 bytes, one byte for red, followed by one byte for green, and so on.

Images are provided in flattened arrays and can be accessed by `RIDX`, defined as

	#define RIDX(i,j,n) ((i)*(n)+(j))

by the code `nameOfImage[RIDX(index1, index2, dimensionOfImage)]`.

All images will be square and have a size that is a multiple of 32.

## What you should change

In `smooth.c` you will see several rotate and several smooth functions.

*	`naive_smooth` which shows you what a naive implementation looks like. Your code must produce the same result as this.

*	You may add as many other smoothmethods as you want. You *should* put each new optimization idea in its own method: `smooth_outer_loop_unrolled_3_times`, `smooth_with_2_by_3_blocking`, etc. The driver will compare all your versions  as long as you register them in the `register_smooth_functions` methods.

The source code you will write will be linked with object code that we supply into a `benchmark` binary. To create this binary, you will need to execute the command

        $ make

You will need to re-make the benchmark program each time you change code in `rotate.c`. To test your implementations, you can run the command:
    
        $ ./benchmark

If you want to only run a particular function, you can run

        $ ./benchmark 'foo'

to only run benchmark functions whose name contains "foo".

***Note that the benchmark results on your machine and the shared lab servers are often different
than our grading machine***. Generally, we've found that optimizations that make code significantly
faster on other machines usually make code significantly faster on our grading machine. However, the amount
of speedup may be quite different.

## Measuring on our grading machine

You can ***measure the performance on our grading machine*** by submitting a version of `smooth.c`
to [kytos](https://kytos.cs.virginia.edu/cs3330/).

We will periodically scan for new submissions and run them on our grading server.

You can view detailed results [here](https://kytos.cs.virginia.edu/cs3330/smoothviewer/), which include the times for each version you submitted.

In addition, your last result will appear on a [public scoreboard](smoothboard.html).

## Correctness Testing

If one of your implementation produces a wrong answer, you can test it with the `./test` program,
which will show you its complete input and output.

To run this, first build it by running

        $ make

then choose a size to test (e.g. `4`), and to test your smooth function named `smooth_bar` use:

        $ ./test "smooth_bar" 4

The `./test` program will run all test functions whose description contains the supplied string. For example,
the above command would run a function whose description was `smooth_bar: version A` 
as well was one whose description was `bad_smooth_bar_and_baz: version B`.

Note that **`./test` can run your program on sizes that are not required to work**. In particular, we do not
require your functions to work on non-multiple-of-32 sizes, but you may find sizes smaller than
32 convenient for debugging.

# Grading

The benchmark program tests your program at several sizes, computes the speedup over the naive
code at each size, then takes the geometric mean of these results to get an overall speedup number.
This is what we will use to grade.
*
Speedups vary wildly by the host hardware. I have scaled the grade based on my timing server's hardware so that particular strategies will get 75% and 100% scores.

Smooth's final submissoin will get 0 points for 1.0× speedups *on our testing computer*, 75% for 3.4×, and 100% for 4.75× speedups, as expressed in the following pseudocode:

	if (speedup < 1.00) return MAX_SCORE * 0;
	if (speedup < 3.40) return MAX_SCORE * 0.75 * (speedup - 1.0) / (3.4 - 1.0);
	if (speedup < 4.75) return MAX_SCORE * (0.75 + 0.25 * (speedup - 3.4) / (4.75 - 3.4));
	return MAX_SCORE;

Smooth's checkpoint submission will be graded similarly, but with thresholds of 1.75 and 2.5
instead of 3.4 and 4.75.

If you submit many times, your grade will be based on the version submitted that results
in the best score, taking late penalties into account.

# Other rules

Violations of the following rules will be seen as cheating, subject to penalties that extend beyond the scope of this assignment:

*	You must not modify or interfere with the timing code 
*	You must not attempt to inspect or modify the network, disk, or other grading platform resources

Additionally, the following rules may result in grade penalties within this assignment if violated:

*	You must write valid C code. (But you may use GCC-specific extensions.)
*	You should not turn in code that contains print statements.
*	Your code must work (i.e., the same functionality as the provided naive implementations) for any image of any multiple-of-32 dimension (32, 64, 96, etc).
*       You must not override command-line optimization options we pass to mostly prevent the compiler from generating vector instructions other than when vector intrinsic are used. That is, you must not use GCC extensions that allow C code in a file to temporarily change the command-line compiler optimizations flags in order to the make the optimizer generate more vector instructions for code that does not use vector intrinsics or otherwise explicitly identify how to use vector instructions.

# About our Testing Server

We will be testing the performance of this program on *our machine*.
We will be build your programs with GCC 7.1.0, which is available on
portal.cs.virginia.edu or via NX remote desktop after running `module load gcc-7.1.0`.
For this compiler
`gcc --version` outputs `gcc (GCC) 7.1.0`. We
will compile your submitted files using the options `-g -Wall -O2 -std=gnu99 -mavx2 -mtune=skylake -fno-strict-aliasing -fno-tree-vectorize`.

Our testing server has an Intel "Kaby Lake" processor with the following caches:

*   A 32KB, 8-way set associative L1 data cache;
*   A 32KB, 8-way set associative L1 instruction cache;
*   A 256KB, 4-way set associative L2 cache, shared between instructions and data;
*   A 8MB, 16-way set associative L3 cache, shared between instructions and data;

The size of a cache block in each of these caches is 64 byte.

Things about our processor that some students might want to know but probably aren't that important:

*   Our processor also has a 4-way set associative 64-entry L1 data TLB, an 8-way set associative 128-entry
L1 instruction TLB, and an 6-way set associative 1536-entry L2 TLB (shared between instructions and
data).
