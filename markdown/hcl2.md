---
title: "HCL2 (irmovq + rrmovq + jmp)"
...

# Your task

1.  Make an HCL file `irrr.hcl` that implements a single-cycle processor with the following instructions:

    -   `nop`
    -   `halt`
    -   `irmovq`
    -   `rrmovq`
    -   unconditional `jmp`

2.  For instructions not in Y86, make your HCL file set `Stat` to `STAT_INS` (invalid instruction error). If an instruction is in Y86 and not listed above, we do not care how your simulator works on it.

3.  Test your file using `make test-irrr`.

4.  Submit your result to the [submission site](https://kytos.cs.virginia.edu/cs3330/)

# Hints

## Suggested method

1.  We suggest implementing and testing one instruction at a time, in the order they're listed above.

2.  Start with an HCL which has a program counter register and increments it based on the instruction `icode` like your `pc.hcl` from the last lab.

## Adding nop/halt

1.  Set `Stat` based on the `icode`. It should be `STAT_HLT` if the `icode` is `halt`, `STAT_INS` for an icode not in Y86 (e.g., > 0xb), `STAT_AOK` otherwise.

## Test nop/halt

1.  If you run your simulator on `y86/halt.yo`, and it should halt after one cycle, and if you run it on `y86/nophalt.yo`, it should halt after 4 cycles.

## Adding irmovq

1.  Extract the `rB` and `valC` fields of the instruction into a wire.

2.  Send `valC` to the register file (`reg_inputE` or `reg_inputM`) as the new value for register number `rB` (`reg_dstE` or `reg_dstM`).

3.  Make sure you do **not** write to the register file when the `icode` is not an `IRMOVQ` or `RRMOVQ`. (Specify `REG_NONE` for `reg_dstE` and/or `reg_dstM`.)

## Testing irmovq

1.  If you run your simulator with the `-q` flag on `y86/irmovq.yo`, you should see something like:

        +----------------------- halted in state: ------------------------------+
        | RAX:              222   RCX:                2   RDX:               22 |
        | RBX:                0   RSP:                0   RBP:                0 |
        | RSI:                0   RDI:                0   R8:                 0 |
        | R9:                 0   R10:                0   R11:                0 |
        | R12:                0   R13:                0   R14: fedcba9876543210 |
        | register pP(N) { pc=0000000000000033 }                                |
        | used memory:   _0 _1 _2 _3  _4 _5 _6 _7   _8 _9 _a _b  _c _d _e _f    |
        |  0x0000000_:   30 f0 01 00  00 00 00 00   00 00 30 f1  02 00 00 00    |
        |  0x0000001_:   00 00 00 00  30 f2 22 00   00 00 00 00  00 00 30 f0    |
        |  0x0000002_:   22 02 00 00  00 00 00 00   30 fe 10 32  54 76 98 ba    |
        |  0x0000003_:   dc fe                                                  |
        +--------------------- (end of halted state) ---------------------------+
        Cycles run: 6

    (It's okay if your `pc` register and register bank has a different name.)

2.  If you see `0` in `RAX` instead of `222` from `y86/irmovq.yo`, it is likely that you're writing to the register file for `halt` as well as `irmovq`.

## Implement `rrmovq`

1.  Extract the `rA` field from the instruction.

2.  When the instruction is `RRMOVQ`, send `rA` to one of the register file source inputs so it reads from this register.

3.  Add `rB` to the one of the register file destination inputs so it writes to that register.

4.  When the instruction is `RRMOVQ`, send the value just read from `rA` into the register file data input signal (`reg_inputE` or `reg_inputM`).

5.  Since you already implemented `irmovq`, several of the above changes will probably require modifying an existing MUX rather than writing new assignments.

## Testing `rrmovq`

1.  If you run your simulator with the `-q` flag on `y86/rrmovq.yo`, you should see these register values:

        | RAX:               22   RCX:               22   RDX:             162e |

2.  Your simulator's register values should agree with `tools/yis` on `y86/prog8.yo`.

## Implement unconditional `jmp`

1.  Add a MUX for `valC`. The `jXX` immediate value is bits `8..72`, not `16..80`.

2.  Add a MUX for the PC register value (or to an existing PC register value MUX) to select `valC` if the `icode` is `JXX`. (You don't need to handle conditional jmps in the lab.)

## Testing unconditional `jmp`

1.  If you run your simulator on `y86/jmp.yo`, you should end up with `ace` and not `bad` in `rax` and nothing in `rbx`.

2.  If you have trouble with the `y86/jmp.yo` test case, see the source code in `y86/jmp.ys` and a trace of the expected cycle-by-cycle output in `testdata/seq-traces/jmp.txt`.

## More complete testing

1.  Run `make test-irrr`. This comapres your simulators output to our references over the files listed in `testdata/irrr-tests.txt`.

## Debugging Test Failures

1.  You can see the expected cycle-by-cycle of a single-cycle procesor on any failing test in `testdata/seq-traces/TESTNAME.txt`. This is our recommendation for debugging test failures

2.  You can run your simulator with the `-d` flag to see the values of all wires during each cycle, which can be helpful when debugging.
