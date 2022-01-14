---
title: Packed structs
...

Both the C and C++ standards allow  bitfields: struct fields specified with a bit width.
For example,

```c
struct S {
    unsigned x:3;
    unsigned y:5;
};
```

is defined to have to fields: a 3-bit `x` and a 5-bit `y`, both of which should behave like an `unsigned int`.
The standard only allows `signed` and `unsigned` types if there's a bit width, though most compilers also allow other type names.

The standard *allows*, but not *not require*, implementations to pack these bits close together.
Thus, a compiler *could* choose to may a `struct S` fit in one byte, or two, or eight, etc: whatever it thinks is most efficient.
This is an example of an **implementation-defined behavior**.

The most common C/C++ compilers implement an extension to tell the compiler "I really wanted things packed up tight":

```c
typedef struct __attribute__((packed)) {
    unsigned x:3;
    unsigned y:5;
} S;
```

If you compile with GCC or Clang (including Apple's variant of Clang), this tells the compiler "don't add extra space in between the bitfields; pack them in tight."

:::aside
Microsoft's C/C++ compiler does not have a reliable way of packing arbitrary bitfields.
It does have [`_declspec(align(1))`](https://docs.microsoft.com/en-us/cpp/build/x64-software-conventions?view=msvc-170#examples-of-structure-alignment) that gets close,
but will sometimes add gaps to prevent bitfields from spanning some byte boundaries.
:::

The C/C++ standards also state that the first attribute of a struct has a smaller address than the second. But that doesn't explain how two values that fit in a single byte should be ordered.
GCC and Clang on little-endian hardware like x86-64 chose to order bitfields 
such that the $2^0$ place is treated as having a smaller address than the $2^1$ place.
This means that the following code

```c
typedef struct __attribute__((packed)) {
    unsigned first:4;
    unsigned second:4;
} Nibbles;
union {
    unsigned char byte;
    Nibbles nibble;
} x;
x.byte = 0x3A;
printf("first = %X; second = %X\n", x.nibble.first, x.nibble.second);
```

will print `first = A; second = 3`: the first field is given the lowest pseudo-address, meaning the lowest-order bits of the byte.

