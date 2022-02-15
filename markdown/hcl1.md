---
title: "HCL1 (pc)"
...

# Your tasks

For each of these tasks, see the more detailed instructions and hints below.

1.  Setup and test HCLRS as described below.

2.  To learn how to use HCLRS and the accompanying Y86 tools for future labs:

    -   Run the supplied `tiny.hcl` file on a `.yo` file (Y86 program)
    -   Assemble a `.ys` into a `.yo` file
    -   Run the supplied Y86 simulator on a yo file

3.  Write a `pc.hcl` based on the supplied `tiny.hcl` that:
    -   advances the PC properly for Y86 instructions that are not jumps, call, or ret, and
    -   sets `Stat` to `STAT_INS` (invalid instruction) for any unimplemented or unrecognized `icode`.

4.  Test your `pc.hcl` using `make test-pc` (many automated tests) and/or by manually running it on input files and comparing to the outputs shown below. On department machines, you will need to run `module load python3` before `make test-pc` will work; on other machines, you will need python3 installed.

5.  **Submit** your `pc.hcl` to the [submission site](https://kytos.cs.virginia.edu/cs3330/)

# More detailed instructions

## Setup and test HCLRS

1.  Get a copy of [hclrs.tar](files/hclrs.tar). See [the hclrs README](hclrs.html) for more information.

2.  Build `hclrs` and the accompanying Y86 tools with `make`

3.  Build the supplied example `tiny` simulator and run it on `y86/prog3.yo` with

    ````sh
    ./hclrs tiny.hcl y86/prog3.yo -q
    ````

        +----------------------- halted in state: ------------------------------+
        | RAX: ffffffffffffd2b4   RCX:                0   RDX:                0 |
        | RBX:                0   RSP:                0   RBP:                0 |
        | RSI:                0   RDI:                0   R8:                 0 |
        | R9:                 0   R10:                0   R11:                0 |
        | R12:                0   R13:                0   R14:                0 |
        | register pP(N) { pc=0000000000000003 }                                |
        | used memory:   _0 _1 _2 _3  _4 _5 _6 _7   _8 _9 _a _b  _c _d _e _f    |
        |  0x0000000_:   30 f2 0a 00  00 00 00 00   00 00 30 f0  03 00 00 00    |
        |  0x0000001_:   00 00 00 00  10 60 20 00                               |
        +--------------------- (end of halted state) ---------------------------+
        Cycles run: 3

    showing the values of the built-in regsiters, memory, and the `pc` register declared by `tiny.hcl`.

## Assemble a `.ys` file into `.yo` file {#assemble-a-ys-file-into-yo-file}

1.  Create a file `toy.ys` with

        irmovq $6552,%rdx
        rrmovq %rdx,%rax
        addq %rax,%rdx
        halt

2.  Assemble `toy.ys` into `toy.yo` using the supplied assembler `tools/yas`

    ````sh
    make toy.yo
    ````

    or

    ````sh
    tools/yas toy.ys
    ````

3.  Examine `toy.yo` in a text editor

## Use the `yis` Y86 simulator

1.  `tools/yis` is a Y86 simulator you can use to see what `.yo` files are supposed to do. For example `tools/yis toy.yo` should give:

        Stopped in 4 steps at PC = 0xe.  Status 'HLT', CC Z=0 S=0 O=0
        Changes to registers:
        %rax:   0x0000000000000000  0x0000000000001998
        %rdx:   0x0000000000000000  0x0000000000003330

        Changes to memory:

    The [hclrs README](hclrs.html) provides more detail about inteprreting this output under the heading "Seeing what a `.yo` file is *supposed* to do".

## Write `pc.hcl` {#write-pchcl}

1.  Copy `tiny.hcl` to `pc.hcl`. Alternately, you can make an HCL file from scratch and with a `pc` register declared like in `tiny.hcl`.

2.  Edit `pc.hcl` so that the pc updates work in the case where there are not jump, call, or return statements. You'll almost certainly want to consult the Y86-64 instruction set described in figure 4.2 (page 357) to do this.

    1.  If you started with `tiny.hcl`, we won't be needing to use the register file in this assignment, so remove the block of hcl that starts with the comment `# let's also read and write a register`... (but keep the line that updates `p_pc`)

    2.  Have the hcl read each instruction and get it's `icode` (`tiny.hcl` does this already)

    3.  Set the `Stat` output to `STAT_INS` (invalid instruction error) if there is a `jXX`, `call`, or `ret` icode; also to `STAT_INS` for any icode greater than 11 (unused icodes); to `STAT_HLT` if there is a `halt` icode; and to `STAT_AOK` for all other `icode`.

        In `tiny.hcl`, there was already a line `Stat = [` that set the `Stat` to either `STAT_AOK` or `STAT_HLT`. You'll need to change it to also set `STAT_INS` for some icodes.

    4.  Update the `p_pc` to be `P_pc +` an appropriate offset (1, 2, 9, or 10, depending on the `icode`).

        There is a line `p_pc = P_pc + 1;` -- you will need to change it so that uses a mux to select what number is added to `pc`.

## Testing `pc.hcl` manually {#testing-pchcl-manually}

Here are some example outputs from running a command like

```sh
./hclrs pc.hcl y86/FILE.yo -q   
```

where `FILE.yo` is replaced with the file you want to run (as in `./hclrs pc.hcl y86/prog1.yo -q`).

-   `y86/prog1.yo` should give

        +----------------------- halted in state: ------------------------------+
        | RAX:                0   RCX:                0   RDX:                0 |
        | RBX:                0   RSP:                0   RBP:                0 |
        | RSI:                0   RDI:                0   R8:                 0 |
        | R9:                 0   R10:                0   R11:                0 |
        | R12:                0   R13:                0   R14:                0 |
        | register pP(N) { pc=000000000000001a }                                |
        | used memory:   _0 _1 _2 _3  _4 _5 _6 _7   _8 _9 _a _b  _c _d _e _f    |
        |  0x0000000_:   30 f2 0a 00  00 00 00 00   00 00 30 f0  03 00 00 00    |
        |  0x0000001_:   00 00 00 00  10 10 10 60   20 00                       |
        +--------------------- (end of halted state) ---------------------------+
        Cycles run: 7

-   `y86/prog7.yo` should give

        +------------------- error caused in state: ----------------------------+
        | RAX:                0   RCX:                0   RDX:                0 |
        | RBX:                0   RSP:                0   RBP:                0 |
        | RSI:                0   RDI:                0   R8:                 0 |
        | R9:                 0   R10:                0   R11:                0 |
        | R12:                0   R13:                0   R14:                0 |
        | register pP(N) { pc=000000000000000b }                                |
        | used memory:   _0 _1 _2 _3  _4 _5 _6 _7   _8 _9 _a _b  _c _d _e _f    |
        |  0x0000000_:   63 00 74 16  00 00 00 00   00 00 00 30  f0 01 00 00    |
        |  0x0000001_:   00 00 00 00  00 00 30 f2   02 00 00 00  00 00 00 00    |
        |  0x0000002_:   30 f3 03 00  00 00 00 00   00 00 00                    |
        +-------------------- (end of error state) -----------------------------+
        Cycles run: 2
        Error code: 4 (Invalid Instruction)

-   `y86/poptest.yo` should give

        +----------------------- halted in state: ------------------------------+
        | RAX:                0   RCX:                0   RDX:                0 |
        | RBX:                0   RSP:                0   RBP:                0 |
        | RSI:                0   RDI:                0   R8:                 0 |
        | R9:                 0   R10:                0   R11:                0 |
        | R12:                0   R13:                0   R14:                0 |
        | register pP(N) { pc=0000000000000019 }                                |
        | used memory:   _0 _1 _2 _3  _4 _5 _6 _7   _8 _9 _a _b  _c _d _e _f    |
        |  0x0000000_:   30 f4 00 01  00 00 00 00   00 00 30 f0  cd ab 00 00    |
        |  0x0000001_:   00 00 00 00  a0 0f b0 4f   00                          |
        +--------------------- (end of halted state) ---------------------------+
        Cycles run: 5

-   Other files. Look in the `testdata/pc-reference` directory to see what our output shows for each of the supplied `.yo` files in the `y86` directory. The outputs in the `testdata/pc-reference` directory omit the final value of the PC register, but your `pc.hcl` should match them in the number of cycles run and error code (if any).

## Testing `pc.hcl` using `make test-pc` {#testing-pchcl-using-make-test-pc}

1.  `make test-pc` will run `pc.hcl` and compare the results to a set of outputs in the `testdata/pc-reference` directory.

2.  When a test fails, the testing program will display the difference between your output and the expected output. This output is a bit hard to understand. The [hclrs README](hclrs.html) has a description of how to read it under the heading "suplied testing scripts".

# Hints

1.  `tiny.hcl` has many comments which may be helpful.
