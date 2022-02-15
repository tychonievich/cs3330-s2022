---
title: "HCL5 (pipelab1)"
...

# Your Task

1.  Download pipelab1_base.hcl (which will be posted on Collab in the resources tab) to get a copy of the sequential simulator with only `nop`, `halt`, `irmovq`, and `rrmovq` implemented.

2.  Rename this to `pipelab1.hcl`, and add pipelining to this simulator with ***two pipeline stages*** that uses ***forwarding*** to handle data hazards:

    -   Stage 1: Fetch and Decode
    -   Stage 2: Writeback

    (There is no execute or memory phase for these instructions.)

3.  Test your simulator with `make test-pipelab1`.

4.  Submit to [kytos](https://kytos.cs.virginia.edu/cs3330/).

# Suggested Approach

We suggest the following steps:

1.  Add a pipeline register bank. Identify where the **two pipeline stages** will split your processor, then identify all wires which cross that point and put them in a pipeline register
2.  Replace wires with pipeline register inputs and outputs wires.
3.  Look for hazards and solve them with forwarding (if possible) or stalling

## Adding the pipeline register bank

Following the textbook's tradition, we suggest calling the input side of the register `d` for decode and the output side `W` for writeback:

    register dW {
        # todo: fill in the details here
    }

Look through `pipelab1_base.hcl`; each value used as an input in writeback that is not also computed in that stage will need to be stored in a pipeline register. For example, the `reg_inputE` mux uses `icode`, `reg_outputA`, and `valC` as inputs, so we'll need all three of those in our new register, as well as registers for the other signals used as inputs in writeback.

Always pick the default values in the pipeline register to be the values you'd expect for `nop`; in `NOP` in the `icode`, `REG_NONE` in any register spots, etc.

## Replacing wires with register inputs/outputs

Recall that if we name our register bank `dW` then whatever signal we put into `d_thing` will come out of `W_thing` on the next cycle.

Go through each signal and, if it crosses the register bank, replace every use before the register bank with `d_...` and every use after the register bank with `W_...`.

For example, consider `icode`:

-   Remove the `wire icode:4` declaration since we have it in `dW`.
-   In fetch and decode, replace all occurrences of `icode` with `d_icode`
-   In writeback replace all occurrences of `icode` with `W_icode`

Do the same thing with `valC`.

### Handling built-in signals

The signals `reg_outputA`, `reg_dstE`, and `Stat` have to be treated specially because they interact with fixed functionality (like the register file). We will need make an explicit connection between the fixed functionality and the appropriate pipeline register since we can't, for example, tell the register file to send its output directly to a pipeline register bank.

Thus, `reg_outputA` (an output set by fixed functionality during decode) will need to be saved into `d_...` during decode and used as `W_...` afterwards, as in

    # in decode:
    d_rvalA = reg_outputA;
    # in execute and later phases, use W_rvalA instead of reg_outputA

Similarly, `reg_dstE` (an input used by fixed functionality during writeback) will be originally computed into `d_dstE` during Decode and then `reg_dstE = W_dstE` placed in writeback to pass the value from the pipeline register to HCLRS's built-in register file.

`Stat` is an output like `reg_dstE` and will need the same treatment (set `d_Stat` before the pipeline register based on the icode, and `Stat = W_Stat` afterward). `Stat` needs to be set in the writeback stage and not the fetch/decode stage, so a `halt` doesn't halt the processor early.

(You could also compute `Stat` from the `icode` in the writeback stage instead of computing it during fetch+decode, but computing the new `Stat` value early may make implementing the full processor easier and matches our textbook's design.)

## Testing without hazard handling

At this point, the `rrmovq.yo` we used for `irrr.hcl`

    irmovq $5678, %rax
    irmovq $34, %rcx
    rrmovq %rax, %rdx
    rrmovq %rcx, %rax

should take 6 (not 5) cycles to set three registers:

    | RAX:               22   RCX:               22   RDX:             162e |

Once you handle `halt` according to the instructions below, it should leave the PC at address 0x18, like the single-cycle processor.

## Handling data hazards

Consider

    irmovq $1, %rax
    rrmovq %rax, %rbx

In a pipeline diagram (given that we have no execute or memory phases), these will look like

  Instr      cycle 1   cycle 2   cycle 3
  --------- --------- --------- ---------
  `irmovq`   FD        W          
  `rrmovq`             FD        W

Note that the immediate value won't be written to the register file until the *after* of cycle 2, but it will be attempted to be read by the next instruction at the *during* of cycle 2. This is an example of a data dependency that exercises a hazard in our hardware design so far. (We will talk about/talked about this on lecture on Tuesday 5 October.)

We can bypass this hazard in two ways. We can either *stall*, or we can *forward* data. To *stall*, we would delay the `rrmovq` instruction until after the `irmovq`'s instruction finishes, causing it to take an extra cycle. To *forward*, we would take the value from the writeback stage of the `irmovq` instruction to the decode stage of the `rrmovq` instruction directly and use it in place of the value we would have otherwise read from the register file.

Since forwarding avoids taking extra cycles, forwarding is always preferred to stalling if both are possible, so we'll forward.

### Forward

We want to grab the value that is being prepped for writing to the register file before it actually gets written if it is the register we are trying to read. Thus, `d_rvalA` (the value of register A being sent from the decode stage to writeback stage) will probably be set by a MUX:

-   if `reg_dstE` is the same as the decode phase's `reg_srcA` and is not `REG_NONE`, it should come from `reg_inputE` (the value that is about to be written);
-   otherwise, it should be `reg_outputA`, as usual

If correctly implemented, `y86/irrr7.yo`

    irmovq $1, %rax
    rrmovq %rax, %rbx

should take 4 cycles to put a 1 in both `%rax` and `%rbx`, while `y86/rrmovq.yo`

    irmovq $5678, %rax
    irmovq $34, %rcx
    rrmovq %rax, %rdx
    rrmovq %rcx, %rax

should still take 6 cycles and result in

    | RAX:               22   RCX:               22   RDX:             162e |

it did before.

I mention the number of cycles because the other solution (stalling) would increase them.

## Handling halt

In the pipelined processors you will be implementing in upcoming homeworks, `halt` and invalid instructions need to be handled carefully since the instructions after a `halt` (or an invalid instruction) are not supposed to run and cause changes to the processor's state.

We recommend keeping the register that feeds the fetch stage the same when you encounter a halt or invalid instruction to avoid starting to executing instructions that aren't part of the program. However, in the two-stage pipeline in this lab, instructions do not change any state (memory, program registers, condition codes) until the last stage, so this is not yet necessary.

(To keep the PC register from changing, you could change the `f_pc` assignment, or use the special `stall_F` signal.)

# Debugging and Miscellaneous Hints

## Pay attention to your pipeline registers

Reading the values of the pipeline registers is a good way to tell what instruction is being run in each stage.

If you declare a register bank like:

    register dW {
        icode : 4 = NOP;
        otherValue : 64 = 0;
    }

Then part of the HCLRS output (when not using `-q`) will have a line like following for every cycle:

    |  register dW(N) { icode=7 otherValue=00000000000000A4 }

indicating that during that cycle, the `W_icode` was `0x7` (the number for JXX, as you can lookup in the [HCLRS manual](hclrs.html) or your textbook) and `W_otherValue` was `0xA4`. With our usual pipeline register naming convention, this means that a JXX insturction is currently in the writeback stage, and the `otherValue` for that instruction was `0xA4`.

## Use debug mode

You can run it in debug mode: `./hclrs -d pipelab1.hcl somefile.yo`. This will show you the value of **all wires** during each step.

You can also redirect the output from debug mode to a file, like:

```sh
./hclrs -d pipelab1.hcl somefile.yo >output.txt 2>&1
```

Then open `output.txt` in a text editor. (`>output.txt` sends normal output to the file `output.txt` instead of the normal output file. `2>&1` means to redirect error output where normal output is going. This redirection support is a shell feature that works with any command, not just `hclrs`.)

## Use reference outputs

You can look at the outputs in `hclrs/testdata/pipelab1-reference` to see what our implementation of `pipelab1.hcl` did for each `.yo` file included with HCLRS.

## Initialize what you declare

The simulator has to provide input to every wire and register in order to run. It does not know what those inputs should be unless you tell it. Thus, if you say

    wire baz:4;
    register qB { xyxxy:32 = 0; }

then you *must* also say

    baz = something;
    q_xyxxy = something_else;

or else you will get an error.

## Test your code often

As with every coding you ever do, **test your code often!** At least check that it compiles with `./hclrs --check pipelab1.hcl`. Particularly when working with a language you don't know well, frequent feedback is useful.
