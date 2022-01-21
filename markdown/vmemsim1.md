---
title: Virtual Memory Simulator
...


Write a single-level page table simulator. You'll have

- 24-bit virtual addresses, for a 16MB virtual address space.
    The high-order 11 bits are a virtual page number;
    the low-order 13 are a page offset.
    
- 20-bit physical addresses, for a 1MB physical address space.
    We'll simulate the RAM chip using a `void *ram` that is aligned to a 1MB boundary (i.e.,  `0 == (ram & 0xFFFFF)`).

Page table entries will by 16 bits (2 bytes) which are

<svg viewBox="-1 -1 322 32" font-size="12" text-anchor="middle" style="max-width:48em">
<rect x="0" y="10" width="20" height="20" fill="none" stroke="black"/>
<text x="10" y="24">X</text>
<rect x="20" y="10" width="140" height="20" fill="none" stroke="black"/>
<text x="80" y="24">PPN</text>
<rect x="160" y="10" width="60" height="20" fill="none" stroke="black"/>
<text x="190" y="24">unused</text>
<rect x="220" y="10" width="20" height="20" fill="none" stroke="black"/>
<text x="230" y="24">D</text>
<rect x="240" y="10" width="20" height="20" fill="none" stroke="black"/>
<text x="250" y="24">A</text>
<rect x="260" y="10" width="20" height="20" fill="none" stroke="black"/>
<text x="270" y="24">U</text>
<rect x="280" y="10" width="20" height="20" fill="none" stroke="black"/>
<text x="290" y="24">W</text>
<rect x="300" y="10" width="20" height="20" fill="none" stroke="black"/>
<text x="310" y="24">P</text>
<g font-size="8">
<text x="10" y="8">15</text>
<text x="25" y="8">14</text>
<text x="155" y="8">8</text>
<text x="165" y="8">7</text>
<text x="215" y="8">5</text>
<text x="230" y="8">4</text>
<text x="250" y="8">3</text>
<text x="270" y="8">2</text>
<text x="290" y="8">1</text>
<text x="310" y="8">0</text>
</g>
</svg>

where

- `P` is 1 if the page is **p**resent, i.e. allocated for this process. If `P` is 0, the other bits in the page table entry are not used by the hardware (they can be used by the OS, but we won't simulate that in this assignment).
- `W` is 1 if the page is **w**ritable (else it's read-only).
- `U` is 1 if the page is accessible in **u**ser-mode (else it's kernel-only).
- `A` (accessed) is set to 1 each time a read or write goes to the page.
- `D` (dirty) is set to 1 each time a write goes to the page.
- `PPN` is the physical page number.
- `X` is 1 if the bytes on the page may be e**x**ecuted as code.

and the physical address is the concatenation of the PPN from the page table entry and the page offset from the virtual address.

We'll also use a special mode bitvector

<svg viewBox="-1 -1 162 32" font-size="12" text-anchor="middle" style="max-width:24em">
<rect x="0" y="10" width="100" height="20" fill="none" stroke="black"/>
<text x="50" y="24">unused</text>
<rect x="100" y="10" width="20" height="20" fill="none" stroke="black"/>
<text x="110" y="24">X</text>
<rect x="120" y="10" width="20" height="20" fill="none" stroke="black"/>
<text x="130" y="24">U</text>
<rect x="140" y="10" width="20" height="20" fill="none" stroke="black"/>
<text x="150" y="24">W</text>
<g font-size="8">
<text x="5" y="8">7</text>
<text x="95" y="8">3</text>
<text x="110" y="8">2</text>
<text x="130" y="8">1</text>
<text x="150" y="8">0</text>
</g>
</svg>

where

- `W` is 1 if the page is being **w**ritten to (else it's being read).
- `U` is 1 if the process is **u**ser-mode (else it's in kernel mode).
- `X` is 1 if the code on the page will be e**x**ecuted.


You will complete the following function that simulates an MMU:

```c
/**
 * Given 
 * 
 * ram    a pointer to the "RAM" (which will always be 1MB-aligned)
 * ptbr   the physical page number of the page table
 * addr   the virtual address to be accessed
 * mode   the access type bitvector
 *
 * either return a pointer to the part of RAM containing that address
 * or NULL if it is not present or not accessible in that way. Also
 * update the A bit, and the D bit if writing.
 */
void *translate(void *ram, unsigned char ptbr, unsigned addr, unsigned char mode);
```

We provide the following simple page-allocation simulator (which does not support deallocation) in our testing files, copied here in case it helps you understand how we are accessing pages:

```c
void allocate(void *ram, unsigned char ptbr, unsigned addr, unsigned char mode) {
    static unsigned nextPage = 0;
    unsigned short *pt = (unsigned short *)(ram+(ptbr<<13));
    pt[addr.vpn] = ((nextPage += 0x31)&0x7F)<<8; // PPN; 0x31 for randomish allocations
    pt[addr.vpn] |= 1; // present
    pt[addr.vpn] |= (mode&4)<<13; // executable
    pt[addr.vpn] |= (mode&3)<<1; // user-mode and writeable
}
```
