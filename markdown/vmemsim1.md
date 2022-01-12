Write a single-level page table simulator. You'll have

- 24-bit virtual addresses, for a 16MB virtual address space.
    The high-order 11 bits are a virtual page number;
    the low-order 13 are a page offset.

- 20-bit physical addresses, for a 1MB physical address space.
    We'll provide this as a `void *` that is aligned to a 1MB boundary (i.e.,  `0 == (ram & 0xFFFFF)`).

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
<text x="270" y="24">K</text>
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

- `P` is 1 if the page is **p**resent, i.e. allocated for this process. If `P` is 0, the meaning of the rest of the bits in the page table entry are defined by the OS and are not used by the hardware.
- `W` is 1 if the page is **w**ritable (else it's read-only).
- `K` is 1 if the page is **k**ernel-only (else it's user-mode).
- `A` (accessed) is set to 1 each time a read or write goes to the page.
- `D` (dirty) is set to 1 each time a write goes to the page.
- `X` is 1 if the bytes on the page may be e**x**ecuted as code.
