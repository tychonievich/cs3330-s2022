---
title: Midterm scope
...

This is a list of topics that I expect familiarity on and that are small enough to be tested in a timed, closed-notes test. In some cases I also note items that we covered that do *not* fit these criteria rule, such as content we'll only test in open-notes ways or content that was an informational aside, not core to the course itself.

*Caveat lector*: I have not intentionally omitted anything, but summaries are not always understood the same way by everyone who reads them. It is possible I thought some specific was implied by something I wrote here that you did not recognize as such.

# Midterm 1

You should be able to

- (in re bits)
	- answer questions about the behavior of provided bit-fiddling code
	- create the building-blocks of bit-fiddling, such as shifts, masks, and setting (to 1), clearing (to 0), and flipping specific bits
	- convert arbitrary-sized binary to/from hex
	- convert 2-digit decimal to/from binary and hex
- (in re assembly)
	- read assembly, including with push, pop, and loops, and answer questions about its behavior
	- convert between assembly and C/C++ code, including loops
	- if given a reminder of the registers involved, correctly call functions in assembly
	- note: only x86-64 is covered in this midterm, not Y86
- (in re indexes and sizes)
	- apply the principle that $2^b = N$ where $b$ is the bits in an index and $N$ is the number of values in the array that it indexes
	- handle power-of-two element sizes, such as converting between indexes into byte arrays and indexes into PTE arrays
	- do $\log_2(x)$ for power-of-2 $x$ and $2^x$ for positive integer $x$ by hand, up to and including $2^{39} = 512G$ and its inverse, and larger values if we provide the appropriate keys (e.g. if we tell you $2^{80} is written "Yi" (short for "yotta"), you should be able to convert $64YiB$ to $2^{86}B$)
- (in re caches)
	- know and apply the rule "cache size = ways × sets × block size"
	- know how direct mapped and fully associative caches relate to set associative caches
	- split addresses into offset, index, and tag
	- apply LRU access rules (but not tree-PLRU)
	- fully simulate by hand the behavior of a given cache, including expressing the full contents of the cache and all its hits, misses, dirty bits, and write actions after a sequence of memory accesses given either explicitly or by code that accesses memory
	- understand and apply write-back and write-through (but not no-write-allocate; we only discussed write-allocate caches this semester)
	- contrast the spatial and temporal locality of arbitrary code or algorithms
	- discuss the purpose and impact of caches
	- apply the concept of caches to the TLB and other non-RAM-backed scenarios we might describe on the exam
- (in re virtual memory)
	- split virtual addresses, physical addresses, and page table entries into their constituent parts, given appropriate parameters to know how big those parts are
	- apply the universal index-and-size rules (above) and page-table specific "make it fit" rules: arrays of PTEs are sized to fit in pages and virtual addresses are sized so that all VPN parts are indices into arrays of PTEs
	- given a PTBR, the contents of RAM, and the parameters describing the sizes of parts of the page table process, do page table traversal by hand, including checking permissions and finding final addresses and values from RAM
	- answer questions about the concept and purpose of virtual address spaces, including how they support concurrent processes, make user code memory-chip-independent, and provide memory protections
- (in re user vs kernel mode)
	- describe the idea of a mode bit register and the implications of an instruction (like "change the mode bit" and "send these bits out on the wire that connects the chip to the disk hardware") or memory page being limited to kernel mode
	- describe how the hardware safely transfers control to the kernel during any exception
	- articulate the differences and similarities between faults, traps, and interrupts and identify which one a given scenario is, even if the scenario described is one we never referenced in class or readings
	- articulate how page tables and kernel mode inter-operate, including how page tables are needed to have kernel-mode memory; how kernel-mode memory is required for safe exception handling; and how kernels can edit page tables while still using virtual addresses to do so
	
