---
title: "HCL8 (pipehw2)"
...

# Your task

1.  Combine your solution from [the previous HW](hcl6.html) and [the previous lab](hcl7.html) into a new file called `pipehw2.hcl` to create a five-stage pipelined processor with forwarding and branch prediction as described in the textbook that implements:

    -   `nop`
    -   `halt`
    -   `irmovq`
    -   `rrmovq`
    -   `OPq`
    -   `cmovXX`
    -   `rmmovq`
    -   `mrmovq`

    We will provide an example lab solution on Collab (under the resources tab) sometime after the lab is due.

2.  Add the remaining instructions:
    -   `jXX`
    -   `pushq`
    -   `popq`
    -   `call`
    -   `ret`

3.  Test your combined simulator with `make test-pipehw2`

4.  Submit your solution to [kytos](https://kytos.cs.virginia.edu/cs3330/)

# Hints/Approach

## General Approach

You may approach this however you wish, but I suggest the following flow:

1.  Combine your `pipehw1.hcl` and `pipelab2.hcl` and test the combination. All of the tests that either source file passed previously ought to still pass the combination.
2.  Add `jXX` with speculative execution and branch misprediction recovery. Predict that all branches are taken. Test.
3.  Add `pushq` and test.
4.  Add `call` and test.
5.  Add `popq` and test.
6.  Add `ret` with handling for the return hazard, and test.

## Implementing `jXX`

             1   2   3   4                     5   6   7    
  --------- --- --- --- --------------------- --- --- --- --- ---
  `jXX`      F   D   E   (next PC available)   M   W            
  `wrong1`       F   D   (bubble needed)                        
  `wrong2`           F   (bubble needed)                        
  `right1`                                     F   D   E   M   W

1.  ***Replace*** `pc` in the `fF` (or `xF` or `pP` or whatever else you called it) register bank wtih `predPC`, which will store a predicted PC value instead of the actual PC value.

    To speculatively use this prediction, we can set `pc` to the predicted PC (`pc = F_predPC`).

2.  Your processor should predict that all `jXX`s are taken (the new PC is `valC`).

3.  We will detect that predictions are wrong near the ***end*** of the `jXX`'s execute stage (when we check the condition codes). We will fetch the correct instruction during the fetch stage in the next cycle (when `jXX` is in the ***memory*** stage).

4.  When we react to a misprediction, we need to:
    -   Squash the mispredicted instructions (which are about to enter the decode and execute stages). You can do this by setting the `bubble_X` signals in the cycle before the corrected instruction is fetched in order to reset the `X_*` pipeline registers to their default values in the next cycle.
    -   Fetch the corrected instruction next cycle (e.g. with a MUX in front of the `pc` signal).

5.  You can fetch the corrected instruction with a MUX front of the `pc` signal:

        pc = [
            mispredicted : oldValP ;
            ...
            1: F_predPC ;
        ];

    You may need to pass the `conditionsMet` signal or something equivalent through a pipeline register to be able to tell when a misprediction happened at the appropriate time.

6.  You will need access to the `valP` from the `jXX` instruction. To do so, you will probably need to pass it through pipeline registers.

7.  Make sure you correctly handle interactions between `jXX` and `halt`. Consider code like:

             jne foo
             halt
        foo: rmmovq %rax, (%rax)
             rmmovq %rax, (%rax)
             rmmovq %rax, (%rax)

    When the `halt` is executed, `F_predPC` may contain the address of an `rmmovq` instead of `halt`: the halt instruction would be fetched because of the `mispredicted` case on the `pc` MUX we suggseted above, rather than being feteched because `F_predPC` pointed to it.

    This means that setting `stall_F` may not be enough to fetch a `halt` next cycle: if we set `stall_F`, then during the next cycle, `F_predPC` will be the same (not halt), but, if using a MUX for `pc` like we suggest above, the value of `mispredicted` and `oldValP` in that MUX will be different.

    Some solutions to this problem may involve using an technique *other than setting `stall_F`* to prevent the PC from changing, like adding a MUX or cases to a MUX for `f_predPC` and/or other MUXes involved in computing the PC.

    For example, one might use code like

        f_predPC = [
            keep_same_instruction_we_fetched : pc ;
            ...
        ];

    to keep the same instruction in the fetch stage when `keep_same_instruction_we_fetched` is set to true rather than setting `stall_F`.

8.  If instead of squashing the mispredicted instructions when they are about to enter the decode and execute stages (like suggested above), you squash them when they are about to enter the execute and memory stages, you will have to worry about preventing the conditions codes from being changed by one of the mispredicted instructions.

## `pushq` and `popq`

1.  `rmmovq` or `mrmovq` except you use `REG_RSP` not `rB` and ±8 not `valC`. Also has a writeback component for `REG_RSP`.

2.  `popq` updates *two* registers, so it will need both `reg_dstE` and `reg_dstM`.

3.  `popq` reads from the old `%rsp` while `pushq` writes to the new `%rsp`.

## `call` and `ret`

      |1|2|3|4|           |5|6|7|8| 
------|-|-|-|-|-----------|-|-|-|-|-
`ret` |F|D|E|M|(next PC available)|W| | | | 
`???` | |F|F|F|(next PC needed)   |F|D|E|M|W


1.  `call 0x1234` is `push valP; jXX 0x1234`. Combining the logic of push and unconditional jump should be sufficient.

2.  `ret` is jump-to-the-read-value-when-popping. It always encounters the "`ret`-hazard":

    You'll have to stall the fetch stage as long as a `ret` is in decode, execute, or memory *and* forward the value from `W_valM` to the `pc`.

## Testing your code

1.  You can run the command `make test-pipehw2` to run your processor on almost all the files in `y86/`, comparing its output to references supplied in `testdata/pipe-reference`. The list of tested files is in `testdata/pipehw2.txt`. For the files `pop-forward2.yo`, `pop-forward3.yo`, `pop-forward4.yo`, `load-store.yo`, you should have the same values, but you may take fewer cycles.

2.  If `make test-pipehw2` produces a lot of output, you can do something like `make test-pipehw2 >test-output.txt 2>&1` to have that output written to the file `test-output.txt` instead of the terminal.

3.  For each input file in `y86/`, there is a trace from our reference implementation in `testdata/pipe-traces`.

4.  Your code should have the same semantics as `tools/yis`: set the same registers and memory. You can use this to see if your processor does the correct thing on any input files, including files you come up with yourself.

5.  We will check the number of cycles your processor takes. As a general rule, your pipelined processor will need

    -   1 cycle per instruction executed
    -   4 extra cycles because we have a five-stage pipeline; even `halt` takes 5 cycles now.
    -   +1 more cycle for each load-use hazard (i.e., read *from memory* in one cycle, use with ALU next cycle)
    -   +2 more cycles for each conditional jump the code should *not* take (the misprediction penalty)
    -   +3 more cycles for each `ret` executed

## Specific Test Cases

### `jXX`

`y86/j-cc.yo`

              irmovq $1, %rsi
              irmovq $2, %rdi
              irmovq $4, %rbp
              irmovq $-32, %rax
              irmovq $64, %rdx
              subq %rdx,%rax
              je target
              nop
              halt
    target:
              addq %rsi,%rdx
              nop
              nop
              nop
              halt

takes 15 cycles and leaves

    | RAX: ffffffffffffffa0   RCX:                0   RDX:               40 |
    | RBX:                0   RSP:                0   RBP:                4 |
    | RSI:                1   RDI:                2   R8:                 0 |


A full trace is available in `testdata/pipe-traces/j-cc.txt`

------------------------------------------------------------------------

`y86/jxx.yo`

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


takes 25 cycles and leaves

    | RAX: ffffffffffffffff   RCX:                0   RDX:                0 |
    | RBX: ffffffffffffffff   RSP:                0   RBP:                0 |


A full trace is available in `testdata/pipe-traces/jxx.txt` (distributed with `hclrs.tar`)

### `pushq`

`y86/push.yo`

    irmovq $3, %rax
    irmovq $256, %rsp
    pushq %rax

    
takes 8 cycles and leaves

    | RAX:                3   RCX:                0   RDX:                0 |
    | RBX:                0   RSP:               f8   RBP:                0 |
    
    | used memory:   _0 _1 _2 _3  _4 _5 _6 _7   _8 _9 _a _b  _c _d _e _f    |
    |  0x000000f_:                              03 00 00 00  00 00 00 00    |


A full trace is available as `testdata/pipe-traces/push.txt`

### `popq`

`y86/pop.yo`

    irmovq $4, %rsp
    popq %rax


takes 7 cycles and leaves

    | RAX:  fb0000000000000   RCX:                0   RDX:                0 |
    | RBX:                0   RSP:                c   RBP:                0 |

A full trace is available as `testdata/pipe-traces/pop.txt`

### `call`

`y86/call.yo`

        irmovq $256, %rsp
        call a
        addq %rsp, %rsp
    a:
        halt

takes 7 cycles and leaves

    | RBX:                0   RSP:               f8   RBP:                0 |

    | used memory:   _0 _1 _2 _3  _4 _5 _6 _7   _8 _9 _a _b  _c _d _e _f    |
    |  0x000000f_:                              13 00 00 00  00 00 00 00    |


A full trace is available as `testdata/pipe-traces/call.txt`

### `ret`

`y86/ret.yo`

        irmovq $256, %rsp
        irmovq a, %rbx
        rmmovq %rbx, (%rsp)
        ret
        halt
    a:
        irmovq $258, %rax
        halt

    
takes 13 cycles and leaves

    | RAX:              102   RCX:                0   RDX:                0 |
    | RBX:               20   RSP:              108   RBP:                0 |

    | used memory:   _0 _1 _2 _3  _4 _5 _6 _7   _8 _9 _a _b  _c _d _e _f    |
    |  0x0000010_:   20 00 00 00  00 00 00 00                               |

A full trace is available as `testdata/pipe-traces/ret.txt`

<small>(It is okay to diagree with this trace about what instruction is fetched and ignored while waiting for the ret, but you should take the same number of cycles and produce the same final results.)</small>

