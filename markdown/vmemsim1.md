---
title: Virtual Memory Simulator
...


Write a single-level page table simulator. You'll have

- 24-bit virtual addresses, for a 16MB virtual address space.
    The high-order 11 bits are a virtual page number;
    the low-order 13 are a page offset.
    
    `typedef struct __attribute__((packed)) { unsigned po:13, vpn:11; } VA;`{.c}^[
        This (and other code on this pace) uses packed bitfields.
        We have a [short writeup](packed_struct.html) explaining that usage.
    ]


- 20-bit physical addresses, for a 1MB physical address space.
    We'll provide this as a `void *` that is aligned to a 1MB boundary (i.e.,  `0 == (ram & 0xFFFFF)`).

    `typedef struct __attribute__((packed)) { unsigned po:13, ppn:7; } PA;`{.c}


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

```c
typedef struct __attribute__((packed)) {
    unsigned p:1, w:1, u:1, a:1, d:1, unused:3, ppn:7, x:1;
} PTE;
```

where

- `P` is 1 if the page is **p**resent, i.e. allocated for this process. If `P` is 0, the other bits in the page table entry are not used by the hardware (they can be used by the OS, but we won't simulate that in this assignment).
- `W` is 1 if the page is **w**ritable (else it's read-only).
- `U` is 1 if the page is **u**ser-mode (else it's kernel-only).
- `A` (accessed) is set to 1 each time a read or write goes to the page.
- `D` (dirty) is set to 1 each time a write goes to the page.
- `X` is 1 if the bytes on the page may be e**x**ecuted as code.

and the physical address is the concatenation of the PPN from the page table entry and the page offset from the virtual address.

We'll also use a special mode flag

```c
typedef struct __attribute__((packed)) {
    unsigned writing:1;
    unsigned user:1;
    unsigned executing:1;
} Mode;
```


You will complete the following function that simulates an MMU:

```c
/**
 * Given 
 * 
 * ram    a pointer to the "RAM" (which will always be 1MB-aligned)
 * ptbr   the physical page number of the page table
 * addr   the virtual address to be accessed
 * mode   the access type: mode&1 write, mode&2 user, mode&4 execute
 *
 * either return a pointer to the part of RAM containing that address
 * or NULL if it is not present or not accessible in that way. Also
 * update the A bit, and the D bit if writing.
 */
void *translate(void *ram, unsigned char ptbr, VA addr, Mode mode);
```

We provide the following simple page-allocation simulator (with no deallocation possible) in our testing files, copied here in case it helps you understand how we are accessing pages:

```c
void allocate(void *ram, unsigned char ptbr, VA addr, Mode mode) {
    static unsigned nextPage = 1;
    PTE *pt = (PTE *)(ram+(ptbr<<13));
    if (!pt[addr.vpn].p) {
        pt[addr.vpn].ppn = nextPage++;
        pt[addr.vpn].p = 1;
    }
    pt[addr.vpn].x = mode.executing;
    pt[addr.vpn].d = 0;
    pt[addr.vpn].a = 0;
    pt[addr.vpn].u = mode.user;
    pt[addr.vpn].w = mode.writing;
}
```
