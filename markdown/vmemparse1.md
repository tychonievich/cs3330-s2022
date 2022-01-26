---
title: Virtual Memory Parser
...

In this lab and its homework pair you will read files containing the entire contents of small physical memory, parse the page tables, and display the resulting memory usage.

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
    
    - To read an array of twenty-five 2-byte big-endian unsigned integers
    
        ````c
        unsgned short result[25];
        fread(result, 2, 25, fp);
        for(int i=0; i<25; i+=1)
            result[i] = (result[i]<<8)&0xFF00|(result[i]>>8)&0x00FF;
        ````

    If necessary, you can also jump around the file using `fseek` and `ftell` and detect the end with `feof`:
    
    - `ftell(fp)`{.c} returns the index of the next byte that would be read by `fread`.
    
    - `fseek(fp, newindex, SEEK_SET)`{.c} moves so that the next `ftell` will return `newindex`.

    - `fseek(fp, delta, SEEK_CUR)`{.c} changes what `ftell` will return by `delta`, skipping `delta` bytes if `delta` is positive or rewinding `-delta` bytes if `delta` is negative.
    
    - `feof(fp)`{.c} is true if you've read the last byte of the file.
        Note that once `feof` returns true, it keeps doing so even if you `fseek` back earlier in the file.
    
    We do not expect most students to use these commands in this assignment, but some approaches might benefit from them.

3. Close the file with `fclose(fp)`{.c}.


## File Format

The files you will be given will have the following structure:

Name   |Bytes  |Format     |Meaning
-------|-------|-----------|----------
magic  |11     |ASCII      |The exact string "3330 VMEM\n\0"
levels |1      |unsigned   |The number of levels in the page table
pob    |1      |unsigned   |The number of bits per page offset
procs  |1      |unsigned   |The number of processes in the file
ptbrs  |1×procs|unsigned\[]|The physical page number of the page table of each process
pages  |1      |unsigned   |The number of pages of physical memory in the file
ram    |$2^{\text{pob}} \times (\text{pages})$|raw bytes  |The entire contents of physical memory

For lab, we will only provide files where *pob* = 13, *procs* = 1, and *levels* = 1.
The code we provide checks *magic* for you, leaving just *pages*, *ptbrs*\[0], and *ram* to handle.

For homework, all numbers may vary.

# Virtual memory layout

Memory is allocated in pages.
Each page either contains a page table, data, or neither.

A page table is an array of page table entries.
We'll use 16-bit (2-byte) page table entries.
Because a (level of a) virtual page number is an index into a page table entry,
(each) VPN is *pob* − 1 bits, meaning the usable part of a virtual address is its low-order *levels* × (*pob* − 1) + *pob* bits.

Page table entries will by 16 bits (2 bytes) which are

<svg viewBox="-1 -1 322 32" font-size="12" text-anchor="middle" style="max-width:48em">
<rect x="0" y="10" width="240" height="20" fill="none" stroke="black"/>
<text x="120" y="24">PPN</text>
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
<text x="25" y="8">14</text>
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


## Getting a file name from the command line

You'll write a program that accepts a filename as a command line argument. Command line argument parsing is not the point of this assignment, so we give that code below:

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) { // verifiy argument count
        fprintf(stderr, "USAGE: %s filenametoparse\n", argv[0]);
        return 1;
    }
    FILE *fp = fopen(argv[1]);
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
