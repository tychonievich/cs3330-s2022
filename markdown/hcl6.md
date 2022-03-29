---
title: "HCL6 (pipehw1)"
...

# Your task

1.  In a file called `pipehw1.hcl`, implement a **five-stage** pipelined Y86 processor (using the pipeline stages described in the textbook) that includes these instructions and uses forwarding to resolve hazards:

    -   `nop`
    -   `halt`
    -   `irmovq`
    -   `rrmovq`
    -   `OPq`, and
    -   `cmovXX`

    (Implement all five stages even though no instructions use the memory stage.)

    You will probably start with your pipelab1 implementation (which includes `nop`, `halt`, `irmovq`, and `rrmovq`, but with only two stages).

2.  Test your implementation with `make test-pipehw1`.

3.  Submit your `pipehw1.hcl` to [kytos](https://kytos.cs.virginia.edu/cs3330/).

# Hints/Approach

You might wish to consult the guides to [HCLRS](hclrs.html) and [Y86-64](y86.html) as well as chapter 4 of the textbook.

## General Approach

1.  Reorganize your solution to [SEQ](hcl4.html) as follows:
    - Add comment labeling each of the five stages
    - Move the existing code into the appropriate stage
    - Verify the reorganized code still works
    
    Most students find it easier to work from SEQ than from PIPE lab 1.

2.  Add five pipeline register banks. 
    1.  Get to know and love figure 4.41 (page 424 of the 3rd edition, page 403 of the 2nd).
    1. Identify where the **five pipeline stages** will split your processor
    2. Name each register bank with two letters:
        - lower-case, a letter representing the stage before the bank
        - upper-case, a letter representing the stage after the bank
        
        To match the textbook's text, the name of these registers would be `fF`, `fD`, `dE`, `eM`, and `mW`.

3.  Work through one instruction at a time, replacing SEQ wires involved in that instruction with PIPE registers and their input and output wires.

4.  Look for hazards and solve them with forwarding (if possible) or stalling

## Stalling for `halt`

You will only need one pipeline stall for this homework: if the `Stat` computed during Fetch is not `STAT_AOK`, you need to prevent additional instructions from being fetched. One way to do this is to adjust the logic that sets `f_pc`. Another is to stall the register bank feeding the fetch stage.

Note that you'll need to pass that non-OK `Stat` down through all the pipeline registers before putting it into the `Stat` output, so this will require 4 cycles during which new instructions are not fetched

When you extend this processor to handle memory instructions, this stall will help you ensure that a program like:

    irmovq $10, %rax
    halt
    rmmovq %rax, 10(%rax)

does not write a value to memory, and otherwise prevent changing the state of your processor because of logically unexecuted instructions.

## Forwarding

For these instructions, `rrmovq` and `OPq` read a register but either `irmovq` or `rrmovq` or `OPq` could be writing it.

One approach, suggested by our textbook and similar to what you did in pipelab1, is to add a mux to the register value inputs `d_valA` and `d_valB` to the `dE` register (like most students do for pipelab1 (though you may called them `rvalA` instead of `valA`)) that checks for things like

    reg_srcA == m_dstE : m_valE;

and so on for all the other possible overlaps of `src`s and `dst`s that are later in the pipeline.

(Another approach would be to add a MUX before the ALU inputs (replacing direct uses of `E_valA` and `E_valB`), checking for things like:

    E_srcA == M_dstE : M_valE;

and so on for all the other possible overlaps of `src`s and `dst`s that are later in the pipeline, and to separately handle forwarding values from writeback to decode.)

## `OPq` and condition codes

1.  Put the condition codes in their own register bank; you don't want to bubble them if you bubble another register bank as part of stalling a stage.

2.  Check the condition codes in execute (based on the `ifun` there) and store the result of that comparison in the `eM` register bank. This will be helpful later when you implement `jXX`.

## `cmovXX` and condition codes

To implement `cmovXX`, all you need to do is change `reg_dstE` to be either the value you'd normally predict for it or `REG_NONE`, depending on the truth of the condition codes. The easiest way to do that is probably by adding a mux in the execute stage, something like

    e_dstE = [
        /* this is a cmovXX and the condition codes are not satisfied */ : REG_NONE;
        1 : E_dstE;
    ];

    
## Orgranizing your code

You may either put all your registers at the beginning of the file, or you may put them between the phases in question. The `hclrs` executable makes multiple passes through your file, so defining them after you use them is permitted and can lead to a more flow-oriented layout, but some people find that distracting and prefer to define before use. Whichever you prefer.

## Specific test cases

Here, we hilight test cases for specific functionality. A common strategy is to get these test cases working in order, then proceed to more complete testing of the processor.

### `nop` and `halt` and illegal instructions

The following program (`y86/halt.yo`)

    halt

should take 5 cycles to complete and do nothing (fetching address 0x0 four times and updating no registers or memory)

------------------------------------------------------------------------

    nop
    nop
    nop
    halt
    nop
 
should take 8 cycles to complete and do nothing (fetching address 0x0, 0x1, 0x2, and then 0x3 five times (never fetch address 0x4) and update no registers or memory)

------------------------------------------------------------------------

The following program (`y86/ins.yo`)

    nop
    iaddq $1, %r8
    halt
 
should take 6 cycles to complete, do nothing, and end with an invalid instruction error (error code 4, `STAT_INS`)

### `irmovq` and `rrmovq`

The following program (`y86/irrr7.yo`)

    irmovq $1, %rax
    rrmovq %rax, %rbx
 
should take 7 cycles to complete and leave 1 in both `%rax` and `%rbx`.

------------------------------------------------------------------------

The following program (`y86/rrmovq.yo`)

    irmovq $5678, %rax
    irmovq $34, %rcx
    rrmovq %rax, %rdx
    rrmovq %rcx, %rax
 
should take 9 cycles to complete and change the following registers:

    | RAX:               22   RCX:               22   RDX:             162e |

------------------------------------------------------------------------

The following program (`y86/irrr7b.yo`)

    irmovq $0x1, %rax
    irmovq $0x2, %rbx
    irmovq $0x3, %rcx
    rrmovq %rax, %rdx
    rrmovq %rcx, %rbx
    rrmovq %rbx, %rsi
    rrmovq %rbx, %rdi

should take 12 cycles and change the following registers:

    | RAX:                1   RCX:                3   RDX:                1 |
    | RBX:                3   RSP:                0   RBP:                0 |
    | RSI:                3   RDI:                3   R8:                 0 |

### `OPq`

The following program (`y86/opq.yo`)

    irmovq $7, %rdx
    irmovq $3, %rcx
    addq %rcx, %rbx
    subq %rdx, %rcx
    andq %rdx, %rbx
    xorq %rcx, %rdx
    andq %rdx, %rsi

should take 12 cycles and leave

    | RAX:                0   RCX: fffffffffffffffc   RDX: fffffffffffffffb |
    | RBX:                3   RSP:                0   RBP:                0 |


A full trace is in `testdata/pipe-traces/opq.txt`.

------------------------------------------------------------------------

The programs `y86/forward1a.yo` through `y86/forward4a.yo` and `y86/forward1b.yo` through `y86/forward4b.yo` systematically test forwarding cases involving `irmovq` and `OPq`.

### `cmovXX`

The following program (`y86/cmovXX.yo`)

    irmovq $2766, %rbx
    irmovq    $1, %rax
    andq    %rax, %rax
    cmovg   %rbx, %rcx
    cmovne  %rbx, %rdx
    irmovq   $-1, %rax
    andq    %rax, %rax
    cmovl   %rbx, %rsp
    cmovle  %rbx, %rbp
    xorq    %rax, %rax
    cmove   %rbx, %rsi
    cmovge  %rbx, %rdi
    irmovq $2989, %rbx
    irmovq    $1, %rax
    andq    %rax, %rax
    cmovl   %rbx, %rcx
    cmove   %rbx, %rdx
    irmovq   $-1, %rax
    andq    %rax, %rax
    cmovge  %rbx, %rsp
    cmovg   %rbx, %rbp
    xorq    %rax, %rax
    cmovl   %rbx, %rsi
    cmovne  %rbx, %rdi
    irmovq    $0, %rbx

should take 30 cycles and leave `0xace` in `%rcx`, `%rdx`, `%rsp`, `%rbp`, `%rsi`, and `%rdi`.

A full trace is available in `testdata/pipe-traces/cmovXX.txt`.

## Other tests

`make test-pipehw1` also runs the following tests, which overlap substantially with the above tests:

-   `prog1.yo` through `prog4.yo` and `prog8.yo`
-   `cmov-noforward.yo`
-   `irmovq-norcxrax.yo`

If `make test-pipehw1` produces a lot of output, you can do something like `make test-pipehw1 >test-output.txt 2>&1` to have that output written to the file `test-output.txt` instead of the terminal.

## Debugging

Our general advice for debugging this assignment:

-   Use the values of the pipeline registers to figure out which instruction is in each pipeline stage;
-   Redirect `hclrs` output to a text file and open it up in a text editor;
-   Use the `-d` output;
-   Write out what instruction is in each stage of the pipeline;
-   Try to simplify the test cases so it's easier to spot your problems;

