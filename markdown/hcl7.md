---
title: "HCL7 (pipelab 2)"
...


# Your Task

1.  In a file called `pipelab2.hcl`, implement a five-stage pipelined processor with forwarding (when possible) and stalling (when needed) for the following instructions:

    -   `rmmovq`
    -   `mrmovq`
    -   `halt`

    We will supply a version of SEQ (`seq_memory.hcl` in the resources tab on Collab) that has just these instructions, or you can start from your seqhw solution.

    We only require you to perform forwarding to the end of the decode stage, even if forwarding a value to the end of the execute stage instead would save a cycle of stalling.

2.  Test with `make test-pipelab2`.

3.  Submit your solution to [kytos](https://kytos.cs.virginia.edu/cs3330/).

# Approach

## Add the registers

We can use the existing `fF` register, since predicting the PC is not needed for these instructions.

At the beginning of decode add `register fD {}`.

At the beginning of execute add `register dE {}`.

At the beginning of memory add `register eM {}`.

At the beginning of writeback add `register mW {}`.

## Wire a signal through the registers

For each stage,

1.  replace read values with outputs from registers in the preceding register bank
2.  replace written values with inputs into the succeeding register bank
3.  make sure you handled fixed-functionality inputs and outputs correctly

Consider SEQ's **decode** stage as it relates to these two instructions:

    reg_srcA = [
        icode in {RMMOVQ} : rA;
        1 : REG_NONE;
    ];
    reg_srcB = [
        icode in {RMMOVQ, MRMOVQ} : rB;
        1 : REG_NONE;
    ];
    reg_dstM = [  /* could also be reg_dstE, which might be easier on the homework */
        icode in {MRMOVQ} : rA;
        1: REG_NONE;
    ];

This stage

1.  Decode uses `icode`, `rA`, and `rB` from the previous stage. That means we'll need to add those to the incoming register bank (`fD`) and replace decode's use of them with the outputs from that bank (e.g., `icode` becomes `D_icode`).

2.  Decode creates three outputs (`reg_srcA`, `reg_srcB`, and `reg_dstM`) so we change them into inputs into the next pipeline register bank (which is `dE`, so `reg_dstM` becomes `d_dstM` instead, etc.). (We'll use `dstM` instead of `dstE` throughout this lab to be consistent with the book, but you can use whatever you find most convenient in your code.)

3.  All three outputs are inputs into the fixed functionality. Did we really want to put them in the pipeline register?

    -   `reg_srcA` is sent to the register file to produces `reg_outputA` as an output. We want to do this register read in Decode, so we should put `d_valA`, not `d_srcA`, into the `dE` register bank.

            reg_srcA = [ ... ];
            d_valA = reg_outputA;

    -   `reg_srcB` is like `reg_srcA`; we want `reg_outputB`, not `reg_srcB`, in `dE` register bank.

    -   `reg_dstM` is half of how we write to the register file (`mem_output` is the other half). We don't want to write to the register file in decode, so we'll use `d_dstM` in Decode and save writing to `reg_dstM` until Writeback

            d_dstM = [ ... ];
            # don't update built-in reg_dstM until writeback...

Work though the same three steps for each of the other stages.

## Forwarding

By this point you should have a more-or-less functioning simulator, but you need to add data forwarding.

One strategy, suggested by our textbook, is to replace `d_valA = reg_outputA` with a check for all possible pending writes in a big mux.

```
d_valA = [
```

First, we do not want to forward if the register is `REG_NONE`:

```
reg_srcA == REG_NONE : 0;
```
    
Where could data come from? Given `mrmovq` is the only possible source, we might need to forward from

-   an `mrmovq` in the Memory stage at the time of Decode, grabbing from the value we are about to put into the `mW` register bank:

    ````
        reg_srcA == m_dstM : m_valM; # forward post-memory (M -> E)
    ````

-   an `mrmovq` in the Writeback stage at the time of Decode, grabbing from the value currently stored in the `mW` register bank:

    ````
        reg_srcA == W_dstM : W_valM; # forward pre-writeback ("register file forwarding")
    ````

-   Or the usual value read from the register file

    ````
        1 : reg_outputA; # returned by register file based on reg_srcA
    ];
    ````

And then do the same thing for `d_valB`...

It is possible to also implement forwarding for `e_valA` in addition to handle the case of a load which is immediately used as a store, but we will not require you to do so in this lab (and our textbook does not do so).

## Stall for the Load-Use Hazard

Even with forwarding we have a data hazard:

  problem                 1   2   3              4                 5   6
  ---------------------- --- --- --- ---------- --- ------------- --- ---
  `mrmovq (%rax), %rcx`   F   D   E              M   (available)   W    
  `mrmovq (%rcx), %rdx`       F   D   (needed)   E                 M   W

Most simply, we can use the `stall_` and `bubble_` signals on our register banks to add a stall to the decode stage.

The `stall_X` signal makes the `X` registers output the same value in the next cycle, ignoring the value being written.

The `bubble_X` signal makes the `X` registers take on their default values, which should represent no-op, in the next cycle.

One could instead manually set the register inputs with MUXes to achieve these effects, too, though that would take many more lines of HCL.

  solution                1   2   3             4                 5   6   7
  ---------------------- --- --- --- --------- --- ------------- --- --- ---
  `mrmovq (%rax), %rcx`   F   D   E             M   (available)   W        
  `mrmovq (%rcx), %rdx`       F   D   (stall)   D   (needed)      E   M   W

This means forwarding is not enough: we need to **stall**.

1.  Detecting the dependency that exercises this hazard.

    The load-use hazard condition is exercised when `mrmovq` is in Execute and its `dstM` is the same as a `srcX` in Decode. We could further constrain ourselves to sources that are going to be used in execute, but we won't for this lab or its following homework: if there is a Decode `src` that matches `mrmovq`'s Execute `dstM`, we'll call it a load-use hazard.

    I suggest creating a `wire loadUse:1;` and initializing it to be true if this hazard is exercised.

2.  Reacting to the hazard when it is exercised.

    We need to stall the decode phase by stalling the previous pipeline register banks and bubbling the one right after decode:

    ````
    /* keep the PC the same next cycle */
    stall_F = loadUse;  /* or add a MUX for f_pc */
        
    /* keep same instruction in decode next cycle */
    stall_D = loadUse;
        
    /* send nop to execute next cycle */
    bubble_E = loadUse;
    ````

    This pattern enables the solution listed above by ensuring every stage ends up with the right work:

      time     F     D                                              E                       M     W
      -------- ----- ---------------------------------------------- ----------------------- ----- -----
      before   ...   `mrmovq (%rcx), %rdx`   `mrmovq (%rax), %rcx`   ...                     ...
      after    ...   `mrmovq (%rcx), %rdx`   `nop`                   `mrmovq (%rax), %rcx`   ...

# Testing your code

## Specific Test Cases

The following assembly (`y86/rrmrb.yo`):

    mrmovq 1(%r8), %rax
    rmmovq %rax, 160(%rax)
    mrmovq 158(%rax), %rdx

    
(which relies on `%r8` initially being `0`) should stall once to take 9 cycles and result in

    +----------------------- halted in state: ------------------------------+
    | RAX:              108   RCX:                0   RDX:          1080000 |

    | used memory:   _0 _1 _2 _3  _4 _5 _6 _7   _8 _9 _a _b  _c _d _e _f    |
    |  0x0000000_:   50 08 01 00  00 00 00 00   00 00 40 00  a0 00 00 00    |
    |  0x0000001_:   00 00 00 00  50 20 9e 00   00 00 00 00  00 00          |
    |  0x000001a_:                              08 01 00 00  00 00 00 00    |
    +--------------------- (end of halted state) ---------------------------+


The following assembly (`y86/mrmr.yo`):

    mrmovq 0x100(%r8), %rax
    mrmovq (%rax), %rcx
    mrmovq (%rcx), %rdx
    .pos 0x100
    .quad 0x200
    .pos 0x200
    .quad 0xfe
 
should stall twice to take 10 cycles and result in

    +----------------------- halted in state: ------------------------------+
    | RAX:              200   RCX:               fe   RDX:          2000000 |

The following assembly (`y86/mrmreasy.yo`):

    mrmovq 0x100(%r8), %rax
    mrmovq 0x200(%r8), %rcx
    mrmovq 0xfe(%r8), %rdx
    .pos 0x100
    .quad 0x200
    .pos 0x200
    .quad 0xfe
 
should not stall at all, take 8 cycles, and result in

    +----------------------- halted in state: ------------------------------+
    | RAX:              200   RCX:               fe   RDX:          2000000 |

    
## Overall Testing

You can run `make test-pipelab2` to run all the tests listed in `testdata/pipelab2-tests.txt`, comparing the outputs to supplied references in `testdata/pipe-references`.

You can see traces for all test cases in `testdata/pipe-traces`.
