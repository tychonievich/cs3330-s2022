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
	- do $\log_2(x)$ for power-of-2 $x$ and $2^x$ for positive integer $x$ by hand, up to and including $2^{39} = 512$G and its inverse, and larger values if we provide the appropriate keys (e.g. if we tell you $2^{80}$ is written "Yi" (short for "yotta"), you should be able to convert $64$YiB to $2^{86}$B)
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
	
# Midterm 2

You should be able to

- (in re Y86)
	- given the maching code layout of the current Y86-64 instructions, answer questions about encoding new instructions
	- know the operation of each instruction, including what it does in each stage of the 5-stage processor
	- understand the relationship between instructions and condition codes
- (in re SEQ)
	- explain how registers and acyclic logic differ, and how they combine to make a general-purpose computer
	- given a new instruction, explain how its implementation differs from that of Y86-64 instructions
	- given a proposed change to the runtime of one functional unit (the ALU, memory, etc), explain how that changes the clock speed and instruction throughput
- (in re pipelining generally)
	- discuss latency, throughput, and the impact of pipelines on both
	- explain the diminishing returns of deep pipelines, including misprediction penalties, register and forwarding overheads, and unequal division of work
	- apply concepts of forwarding and stalling to track the cycles used by instruction sequences for any given set of pipeline stages, including filling in pipeline diagrams
	- apply per-register-bank stall and bubble signals to stall and/or squash specific instructions in a pipeline
	- explain the difference and relationships between hazards, control dependencies, and data dependencies
- (in re PIPE)
	- know, identify, and explain the pipeline performance of the load-use and return hazards and branch misprediction
	- apply the "always taken", "never taken", and "backward-taken forward-not-taken" branch prediction heuristics
	- discuss the impact of proposed changes to PIPE, such as
		- splitting a stage into several stages
		- merging stages into a single stage
		- modifying forwarding logic
		- adding functional units to accomodate new instructions
		- giving some stages a variable-number-of-cycles runtime, such as the cache hierachy might create
	- identify specifically what is forwarded from what stage to what stage
	- discuss how PIPE would adapt to adding or removing instructions from its ISA
- (other)
	- discuss the basics of multiple issue, including applying relevant concepts from pipelines (such as data and control dependencies) to them
    - given a sequence of Y86-64 instructions, a mapping between architectural and physical registers, and a list of free registers, perform register renaming


# Past Exams

| Name | Blank | Key | Min | Mean | Max | Gamma correction[^gamma] |
|------|-------|-----|:---:|:----:|:---:|:------------------------:|
| Exam 1 | [pdf](files/mt1.pdf)[^typo1] | [pdf](files/mt1key.pdf) | 42% | 76% | 97% | awarded = raw^0.7^ |
| Exam 2 | [pdf](files/mt2.pdf) | [html](s2022mt2key.html) | 53% | 80% | 96% | awarded = raw^0.7^ |

[^typo1]:
    The last question has a typo.
    If says there are four pages, but then only gives three.
    In a question about counting pages, that typo was significant enough to invalidate the question as a measure of learning.
    
    If the text had said three pages to match the bullets, then the correct answer would have been 13.
    If it had given the VPN of a fourth page, the answer would have been between 14 and 17, depending on what the fourth VPN was.


[^gamma]:
    Exams are a finite precision encoding (because they have a finite number of questions) of a bounded-range but continuous number (student understanding of course material, from nothing (0%) to everything we covered (100%)).
    Linear encodings ("raw score") waste most of that precision on distinguishing between small numbers (more than half of the precision in distinguishing between different kinds of "F"), leaving few points left to distinguish between more common scores like A and B.
    I instead use a gamma-corrected encoding, a standard technique to encode numbers between 0 and 1 that keeps the full range (0 and 1 are unchanged) while putting more precision at either the low (gamma > 1) or high (gamma < 1) end of the range.
    
    For optimal precision, I'd want the raw score of a student who should pass but has significant room for improvement (i.e., C-level knowledge) to be 40–50%, adjusted by a gamma around 0.35–0.4.
    That will put most of the precision in the passing range and make the results for passing students relatively robust against small errors.
    However, that kind of test is very disheartening to take.
    On the other extreme, using a gamma of 1.0 makes the test feel easy and boosts confidence, but reduces precision for most students and means a mistake on just a question or two can change your grade by a full letter.
    
    I generally try to split the difference, aiming for a gamma somewhere around 0.65 when writing the test: every question should be tricky enough to reveal depth of student knowledge, and C-level knowledge should be enough earn about ⅔ of the available points.
    I then go through the questions and count up the raw score I expect a C-level understanding to earn and find the actual gamma needed to bring that raw score into the C range.
    
    Only after picking the gamma do we begin grading.
    Picking the gamma first ensures I focus on *adequate* rather than *average* understanding.
    Average-based grading makes it impossible for the entire class can earn an A and that creates perverse incentives where self-interest is served by ensuring other students do poorly.
    I don't like those incentives, hence the predetermined gamma correction instead.
