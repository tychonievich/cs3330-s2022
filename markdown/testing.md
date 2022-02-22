---
title: Midterm scope
...

This is a list of 

- topics that I expect familiarity on
- topics that we mentioned but were supplemental, not core, and I do not expect familarity on
- topics that are important, but too time-consuming to evaluate in a test

I have not intentionally omitted anything, but summaries are not always understood the same way by everyone who reads them.

# Midterm 1

You should be able to

- bits
	- answer questions about the behavior of provided bit-fiddling code
	- create the building-blocks of bit-fiddling, such as shifts, masks, and setting (to 1), clearing (to 0), and flipping specific bits
	- convert arbitrary-sized binary to/from hex
	- convert 2-digit decimal to/from binary and hex
- assembly
	- read assembly, including with push, pop, and loops, and answer questions about its behavior
	- convert between assembly and C/C++ code, including loops
	- if given a reminder of the registers involved, correctly call functions in assembly
	- note: only x86-64 is covered in this midterm, not Y86
- indexes and sizes
	- apply the principle that $2^b = N$ where $b$ is the bits in an index and $N$ is the number of values in the array that it indexes
	- handle power-of-two element sizes, such as converting between indexes into byte arrays and indexes into PTE arrays
- caches
	- know and apply the rule "cache size = ways × sets × block size"
	- know how direct mapped and fully associative caches relate to set associative caches
	- split addresses into offset, index, and tag
	- apply LRU access rules (but not tree-PLRU)
	- fully simulate by hand the behavior of a given cache, including expressing the full contents of the cache and all its hits, misses, dirty bits, and write actions after a given sequence of memory accesses given either explicitly or by code that accesses memory
	- understand and apply write-back and write-through (but not no-write-allocate; we only discussed write-allcoate caches this semester)
	- contrast the spatial and temporal locality of arbitrary code or algorithms
	- discuss the purpose and impact of caches
	- apply the concept of caches to things other than the giant byte array that is RAM, including the TLB
- virtual memory
	- split virtual addresses, physical addresses, and page table entries into their constituent parts, given appropriate parameters to know how big those parts are
	- apply the universal index-and-size rules (above) and page-table specific "make it fit" rules: arrays of PTEs are sized to fit in pages and virtual addresses are sized so that all VPN parts are indices into arrays of PTEs
	- given a PTBR, the contents of RAM, and the parameters describing the sizes of parts of the page table process, do page table traveral by hand, including checking permissions and finding final addresses and values from RAM
	- understand virtual address spaces, including how they support concurrent processes, make user code memory-chip-independant, and provide memory protections
- user vs kernel mode
	- understand the idea of a mode bit register and the implications of an instruction (like "change the mode bit" and "send these bits out on the wire that connects the chip to the disk hardware") and memory being limited to kernel mode
	- understand how the hardware safely transfers control to the kernel during any exception
	- articulate the differences and similarities between faults, traps, and interrupts and identify which one a given scenario is, even if the scenario described is one we never referenced in class or readings
	- articulate how page tables and kernel mode interoperate, including how page tables are needed to have kernel-mode memory; how kernel-mode memory is required for safe exception handling; and how kernels can edit page tables while still using virtual addresses to do so
	
