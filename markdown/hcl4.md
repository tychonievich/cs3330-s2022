---
title: "HCL4 (seqhw)"
...

# Your task

1.  Start from the single-cycle processor you implemented in the [previous lab](hcl3.html). Make a copy of this file called `seqhw.hcl`.

2.  Implement all the remaining Y86 instructions in `seqhw.hcl`:
    -   `jXX` (conditional jumps)
    -   `mrmovq`
    -   `pushq`
    -   `popq`
    -   `call`
    -   `ret`

3.  Test your code with `make test-seqhw`.

4.  Submit to the [submission site](https://kytos.cs.virginia.edu/cs3330/)

# Advice/Hints

You might wish to consult the guides to [HCLRS](hclrs.html) and [Y86-64](y86.html) as well as chapter 4 of the textbook.

## `Stat`

The `Stat` should be

-   `STAT_INS` if the `icode` is not one of the ones the book discusses.
-   `STAT_HLT` if the `icode` is `halt`.
-   `STAT_AOK` otherwise.

## Implementing `jXX`

To add conditional support to JXX, you should update the PC to the immediate value (`valC`) only if the conditions are met. You should already have something like `wire conditionsMet:1` from implementing `cmovXX` in lab.

## Testing `jXX`

If `jXX` is correctly implemented, the following (which is found in y86/jxx.yo) should run for 19 steps, visiting hex addresses 0, a, 14, 27, 29, 1d, 14, 27, 29, 1d, 14, 27, 29 1d, 14, 27, 29, 1d, and 26, then halting at address 26:

        irmovq $3, %rax
        irmovq $-1, %rbx
    a:
        jmp b
    c:
        jge a
        halt
    b:
        addq %rbx, %rax
        jmp c

    
You can test this by running the output of the simulator through the `grep` tool to select out just a subset of lines:

    linux> ./hclrs seqhw.hcl y86/jxx.yo | grep 'pc ='
    pc = 0x0; loaded [30 f0 03 00 00 00 00 00 00 00 : irmovq $0x3, %rax]
    pc = 0xa; loaded [30 f3 ff ff ff ff ff ff ff ff : irmovq $0xffffffffffff, %rbx]
    pc = 0x14; loaded [70 27 00 00 00 00 00 00 00 : jmp 0x27]
    pc = 0x27; loaded [60 30 : addq %rbx, %rax]
    pc = 0x29; loaded [70 1d 00 00 00 00 00 00 00 : jmp 0x1d]
    pc = 0x1d; loaded [75 14 00 00 00 00 00 00 00 : jge 0x14]
    pc = 0x14; loaded [70 27 00 00 00 00 00 00 00 : jmp 0x27]
    pc = 0x27; loaded [60 30 : addq %rbx, %rax]
    pc = 0x29; loaded [70 1d 00 00 00 00 00 00 00 : jmp 0x1d]
    pc = 0x1d; loaded [75 14 00 00 00 00 00 00 00 : jge 0x14]
    pc = 0x14; loaded [70 27 00 00 00 00 00 00 00 : jmp 0x27]
    pc = 0x27; loaded [60 30 : addq %rbx, %rax]
    pc = 0x29; loaded [70 1d 00 00 00 00 00 00 00 : jmp 0x1d]
    pc = 0x1d; loaded [75 14 00 00 00 00 00 00 00 : jge 0x14]
    pc = 0x14; loaded [70 27 00 00 00 00 00 00 00 : jmp 0x27]
    pc = 0x27; loaded [60 30 : addq %rbx, %rax]
    pc = 0x29; loaded [70 1d 00 00 00 00 00 00 00 : jmp 0x1d]
    pc = 0x1d; loaded [75 14 00 00 00 00 00 00 00 : jge 0x14]
    pc = 0x26; loaded [00 : halt]


## Implementing `mrmovq`

1.  Memory is accessed by setting `mem_addr` to the memory address in question and either
    -   setting `mem_readbit` to `0`, `mem_writebit` to `1`, and `mem_input` to the value to write to memory, which will cause the memory system to write a 8-byte value to memory; or
    -   setting `mem_readbit` to `1` and `mem_writebit` to `0`, which will cause the memory system to read a 8-byte value from memory into `mem_output`.
2.  You will also need to compute the memory address as `reg_outputB` + `valC` (the book suggests you do this in the ALU, meaning the same mux you used for `OPq`'s adding and subtracting). This is the same calculation used for `rmmovq`.

## Testing `mrmovq`

If both memory moves are implemented correctly, the following (`y86/rrmr.yo`) should result in `%rdx` containing 0x20000 and address 0xa2 containing byte 0x02.

    mrmovq 2(%rax), %rax
    rmmovq %rax, 160(%rax)
    mrmovq 158(%rax), %rdx


The first instruction relies on `%rax` initially being `0`.

## Implementing `pushq`

Decode: read `rA` and `%rsp`

Execute: add −8 to `%rsp`

Memory: write `reg_outputA` to the address computed by that subtraction

Writeback: write the result of the subtraction back into `%rsp`

## Testing `pushq`

The following code (`y86/push.yo`)

    irmovq $3, %rax
    irmovq $256, %rsp
    pushq %rax

should leave a 0x03 in address 0xf8 and an 0xf8 in `%rsp`

## Implementing `popq`

Decode: read `%rsp`

Execute: add +8 to `%rsp`

Memory: read from the pre-add `%rsp` address (the value read in decode, not the value computed during execute)

Writeback: write both (1) the result of the addition back into `%rsp` and (2) the results of the read into `rA`

## Testing `popq`

The following code (`y86/pop.yo`)

    irmovq $4, %rsp
    popq %rax

should leave a 0xc in `%rsp` and a 0xfb0000000000000 in `%rax`

## Implementing `call`

`call` is like `pushq` and `jmp` in general form

Decode: read `%rsp`

Execute: add −8 to `%rsp`

Memory: write the next instruction address (`valP`) to the address computed by that subtraction

Writeback: write the result of the subtraction back into `%rsp`

PC Update: the new PC (`p_pc`) should be `valC`, not `valP`.

## Testing `call`

The following code (`y86/call.yo`)

        irmovq $256, %rsp
        call a
        addq %rsp, %rsp
    a:
        halt

should leave 0xf8 in `%rsp` and a 0x13 in address 0xf8

## Implementing `ret`

`ret` is like `popq` and `jmp` in general form

Decode: read `%rsp`

Execute: add +8 to `%rsp`

Memory: read from the pre-added `%rsp` address

Writeback: write the result of the addition back into `%rsp`

PC Update: the new PC (`p_pc`) should be the value read from memory (`mem_output`), not `valP`.

## Testing `ret`

The following code (`y86/ret.yo`)

        irmovq $256, %rsp
        irmovq a, %rbx
        rmmovq %rbx, (%rsp)
        ret
        halt
    a:
        irmovq $258, %rax
        halt

    
should run 6 cycles, leave `%rax` as 0x102, and `%rsp` as 0x108

## General Testing

You can run the command `make test-seqhw` to test your processor over almost all the `.yo` files in the `y86/` folder, comparing the output to supplied outputs in `testdata/seq-reference` and `testdata/seq-traces`. If your processor disagrees, you may find the traces in `testdata/seq-traces` helpful for debugging.

If `make test-seqhw` produces a lot of output, you can do something like `make test-seqhw >test-output.txt 2>&1` to have that output written to the file `test-output.txt` instead of the terminal.

In addition, your code should behave the same as `tools/yis` when run on anything in the `y86/` folder *except*

-   `asumi.yo` and `ins.yo` (which use instructions not in the y86 basic set). Note that you should still agree with our reference files (that `make test-seqhw` uses) for these tests.
-   `pushquestion.yo` (which is ambiguous and may or may not work the same as `yis`; we don't care either way), `poptest.yo` (which is similarly ambiguous)
-   `prog10.yo` (which gives an address error, a limitation we are not implementing in our simulator).
