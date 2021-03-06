---
title: Virtual Memory Parser
...

In this lab and its homework pair you will read files containing the entire contents of small physical memory, parse the page tables, and display the resulting memory usage.

- Starter code: [vmemparse.c](files/vmemparse.c)
- Lab input file [linux.ram](files/linux.ram) and its [reference output](files/linix.ram.svg)
- Additional files for homework:
    - [blank.ram](files/blank.ram) [blank.ram.svg](files/blank.ram.svg) -- no allocated pages
    - [linux2.ram](files/linux2.ram) [linux2.ram.svg](files/linux2.ram.svg) -- two processes
    - [smallvm.ram](files/smallvm.ram) [smallvm.ram.svg](files/smallvm.ram.svg) -- different memory sizes

# Binary File Format

Many real file formats are "binary" formats, meaning that they are not textual and cannot be viewed in a text editor.
You'll learn the basics of reading binary files in this lab.

## How to read

There are multiple file reading APIs; we'll use C's standard `FILE *` API declared in `#include <stdio.h>`{.c}:

1. `FILE *fp = fopen(filename, "rb");`{.c} The `"rb"` means to open the file for reading in binary mode. The "in binary mode" *probably* doesn't matter, as almost all systems default to that now, but it's safer to include it just in case.

2. Read data, from the first byte to the last in the file, using `fread`. Run `man fread` to learn how this works. A few examples:

    - To read one 4-byte little-endian integer:
        
        ````c
        int result;
        fread(&result, 4, 1, fp);
        ````
    
    - To read an array of twenty-five 2-byte little-endian unsigned integers:
    
        ````c
        unsigned short *result = malloc(sizeof(short)*25);
        fread(result, 2, 25, fp);
        // ... use result here ...
        free(result);
        ````
    
    - To read a single byte you can use either `fread` or `fgetc`:
        
        ````c
        unsigned char a, b;
        fread(&a, 1, 1, fp);
        b = fgetc(fp);
        ````

    If necessary, you can also jump around the file using `fseek` and `ftell` and detect the end with `feof`:
    
    - `ftell(fp)`{.c} returns the index of the next byte that would be read by `fread`.
    
    - `fseek(fp, newindex, SEEK_SET)`{.c} moves so that the next `ftell` will return `newindex`.

    - `fseek(fp, delta, SEEK_CUR)`{.c} changes what `ftell` will return by `delta`, skipping `delta` bytes if `delta` is positive or rewinding `-delta` bytes if `delta` is negative.
    
    - `feof(fp)`{.c} is true if you've read the last byte of the file.
        Note that once `feof` returns true, it keeps doing so even if you `fseek` back earlier in the file.
    
    We do not expect most students to use these commands in this assignment, but some approaches might benefit from them.

3. Close the file with `fclose(fp)`{.c}.
    
    Closing files is polite (it returns resources to the OS before the program finishes), but not technically necessary when reading a file.
    When writing to a file, closing it is necessary: the OS reserves the right to delay actually putting data on disk until the file is closed.


## File Format

The files you will be given will have the following structure:

Name   |Bytes  |Format     |Meaning
-------|:-----:|:---------:|----------
magic  |11     |ASCII      |The exact string `"3330 VMEM\n\0"`^[Many file formats begin with some quasi-unique set of bytes called the file format's "magic number". This can help tools recognize file formats even if they are provided without other identifying information.]
levels |1      |unsigned   |The number of levels in the page table; always 1 in this assignment
pob    |1      |unsigned   |The number of bits per page offset
procs  |1      |unsigned   |The number of processes in the file
ptbrs  |1??????procs|unsigned\[]|The physical page number of the page table of each process
pages  |1      |unsigned   |The number of pages of physical memory in the file
ram    |2^pob^??????pages|raw bytes  |The entire contents of physical memory

For lab, we will only provide files where *pob*??=??13, *procs*??=??1, and *levels*??=??1.
The [code we provide](#startercode) checks *magic* for you, leaving just *pages*, *ptbrs*\[0], and *ram* to handle.

For homework, all numbers may vary.

# Virtual memory layout

Memory is allocated in pages.
Each page either contains a page table, data, or neither.

A page table is an array of page table entries.
We'll use 16-bit (2-byte) page table entries.
Because a (level of a) virtual page number is an index into a page table entry,
(each) VPN is *pob*???????1 bits, meaning the usable part of a virtual address is its low-order *levels*??????(*pob*???????1)??+??*pob* bits.

Page table entries will be 16 bits (2 bytes) which are

<svg viewBox="-1 -1 322 32" font-size="12" text-anchor="middle" style="max-width:48em">
<rect x="0" y="10" width="240" height="20" fill="none" stroke="black"/>
<text x="120" y="24">zero-padded PPN</text>
<rect x="240" y="10" width="20" height="20" fill="none" stroke="black"/>
<text x="250" y="24">X</text>
<rect x="260" y="10" width="20" height="20" fill="none" stroke="black"/>
<text x="270" y="24">W</text>
<rect x="280" y="10" width="20" height="20" fill="none" stroke="black"/>
<text x="290" y="24">U</text>
<rect x="300" y="10" width="20" height="20" fill="none" stroke="black"/>
<text x="310" y="24">P</text>
<g font-size="8">
<text x="5" y="8">15</text>
<text x="235" y="8">4</text>
<text x="250" y="8">3</text>
<text x="270" y="8">2</text>
<text x="290" y="8">1</text>
<text x="310" y="8">0</text>
</g>
</svg>

where

- `P` is 1 if the page is **p**resent, i.e. allocated for this process. If `P` is 0, the other bits in the page table entry are not used by the hardware (they can be used by the OS, but we won't simulate that in this assignment).
- `U` is 1 if the page is accessible in **u**ser-mode (else it's kernel-only).
- `W` is 1 if the page is **w**ritable (else it's read-only).
- `X` is 1 if the bytes on the page may be e**x**ecuted as code.
- `PPN` is the physical page number. If there are more bits than needed, it is 0-padded on the high-order bits to fit in the given bits.

and the physical address is the concatenation of the PPN from the page table entry and the page offset from the virtual address.


## Getting a file name from the command line {#startercode}

You'll write a program that accepts a filename as a command line argument. Command line argument parsing is not the point of this assignment, so we give that code in the [starter file](files/vmemparse.c).

<details><summary>The argument parsing code</summary>
```c
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) { // verifiy argument count
        fprintf(stderr, "USAGE: %s filenametoparse\n", argv[0]);
        return 1;
    }
    FILE *fp = fopen(argv[1], "rb");
    if (!fp) { // verify that the argument was a file
        fprintf(stderr, "Cannot open \"%s\" for parsing\n", argv[1]);
        return 1;
    }
    
    // read the magic starting string
    char magic[11];
    if (11 != fread(magic, 1, 11, fp)) {
        fprintf(stderr, "Invalid file format: \"%s\" too short\n", argv[1]);
        return 1;
    }
    if (strncmp(magic, "3330 VMEM\n\0", 11)) {
        fprintf(stderr, "Invalid file format: \"%s\" missing magic number\n", argv[1]);
        return 1;
    }

    /*
     * Your code here
     */
    
    fclose(fp);
    return 0;
}
```
</details>

# Drawing the contents of memory

Your task in both the lab and homework
is to make a program that can read a binary file describing the contents of RAM, as outlined above
and output an SVG file providing an illustration of both physical and virtual memory layout.
SVG is a convenient graphics format because

- It is [viewable by all major web browsers](https://caniuse.com/?search=svg)
- It is textual, not binary, making it easier to learn to write
- It contains descriptions of shapes to draw rather than pixels to color

That said, we provide the actual SVG creation code for you in the [starter file](files/vmemparse.c). In particular, we provide four functions:

- `void startImage(FILE *dst, unsigned char pages, unsigned char vpnbits, unsigned char processes)`
    
    This creates the SVG header, sizes the image, and creates the background shapes and labels. Call it on a newly-opened writable file (e.g., created by `fopen("filename.svg", "w")` before calling the other image functions we provide.

- `void drawVPNPage(FILE *dst, unsigned process, unsigned page, int isKernel, int isCode, int isWriteable)`

    This draws a stripe of color representing the given virtual address.
    Kernel pages are drawn darker than user pages.
    Code is shown in green, read/write data in blue, read-only data in gray or white.

- `void drawPPNPage(FILE *dst, unsigned page, char label)`

    This puts a single-character label in the given page of physical memory.
    Use digits for pages owned a given process (i.e., `'1' + processId`)
    and `'T'` for page tables.

- `void endImage(FILE *dst)`

    This creates the SVG trailer. Call it after you're done filling the image file with content, but before calling `fclose`.


# Lab Task

Create code that, when given [`linux.ram`](files/linux.ram), creates the following image:

![Correct image for `linux.ram`](files/linux.ram.svg){width="15em"}

Recommended steps:

1. The starter code already checks the magic number.
    You should read the levels and so on from the rest of the header.
    
    We recommend reading the entirety of the `ram` field into a `malloc`-allocated array.
    
2. Call `startImage`

3. Call `drawPPNPage` to put a `T` on the physical page of the first (and for the lab only) PTBR.

4. Read each PTE in the page indicated by the PTBR and for each
    
    a. Check if the page is present; if not, skip it
    
    b. The index of this PTE is the VPN to draw; read the other flag bits and call `drawVPNPage`
    
    c. Also call `drawPPNPage` using the PPN in the PTE.

5. Call `endImage`

6. `free` anything you `malloc`ed


# Homework Task

Also make your code work for any input where `levels == 1` and `procs >= 1`.

Note: if there are 2+ processors, it is possible they might both allocate the same page of physical RAM.
This is called "shared memory" and is used for kernel code, shared libraries, and some types of inter-process communication.
when that happens in an input file, label the given physical page multiple times;
the numbers will overlap in the image, showing the shared state of the page.

