---
title: "HCL3 (seqlab)"
...


# Fix permissions

Some student accounts were set up with the wrong file permissions.

Please visit [the permission checker page](https://www.cs.virginia.edu/luther/permcheck/) and follow any instructions it gives you.


# Your task

1.  Start with your [HCL2](hcl2.html) solution that implements `irmovq`, `rrmovq`, `halt`, and unconditional `jmp`. (If you don't have a working HCL2 solution, fix it first.)

2.  Copy your HCL2 solution to new HCL file called `seqlab.hcl` in the `hclrs` directory.

3.  Add `OPq`, `cmovXX`, and `rmmovq` to the single-cycle processor in that HCL file. For `OPq` and `cmovXX`, you only need to implement the `SF` and ZF\` condition codes. (We don't care about overflow.)

4.  Test your solution with `make test-seqlab`.

5.  Submit your `seqlab.hcl` to the [submission site](https://kytos.cs.virginia.edu/cs3330/).

# Advice/Hints

## Implementing `OPq`

The textbook's Figure 4.18 (page 387) notes the following semantics for `OPq`:

  -----------------------------------
  Stage     `OPq` rA, rB
  --------- -------------------------
  Fetch     icode:ifun ← M~1~\[PC\]\
            rA:rB ← M~1~\[PC + 1\]\
            valP ← PC + 2

  Decode    valA ← R\[rA\]\
            valB ← R\[rB\]

  Execute   valE ← valB OP valA\
            Set CC

  Memory     

  Writeback R\[rB\] ← valE

  PC Update PC ← valP
  -----------------------------------

(M~1~\[X\] indicates one byte read from memory from a particular address --- in this case, part of the instrution memory's output; R\[X\] represents reading the register with number X; CC represents the condition codes; PC represents the program counter register's input or output.)

All the interesting stuff is in the execute phase, including:

-   Performing the appropriate arithmetic operation.
-   Handling the condition codes `ZF` and `SF`, which you will need later for `cmovXX`.

### The ALU operation

1.  The ALU operation is essentially a MUX based on `ifun` (the least significant nibble of the first byte of the instruction) with lines inside it like `icode == OPQ && ifun == XORQ : reg_outputA ^ reg_outputB;`.

### The condition codes

1.  Create a register to store the condition codes inside of, like:

         register cC {
             SF:1 = 0;
             ZF:1 = 1;
         }

    (`ZF` defaulting to 1 is consistent with `yis`, but we won't test what you choose as the initial value of the condition codes.)

2.  Record if the (signed) value of `valE` is \<, =, or > 0 (using unsigned comparison operators) in the condition codes.

         c_ZF = (valE == 0);
         c_SF = (valE >= 0x8000000000000000);

3.  You must only update condition codes during an `OPq` instruction; other operations should not update them. Register banks like `cC` have a special input `stall_C` which, if `1`, causes the registers to ignore inputs and keep their current value. (The `C` in `stall_C` matches the capital letter of the register bank name `cC`; the signal would be named differently for other register banks.) So, we can use this to avoid updating the condition codes unless there's an OPq:

        stall_C = (icode != OPQ);

## Testing `OPq`

1.  If you run your simulator on y86/opq.yo, which is an assembled version of

        irmovq   $7, %rdx
        irmovq   $3, %rcx
        addq %rcx, %rbx
        subq %rdx, %rcx
        andq %rdx, %rbx
        xorq %rcx, %rdx 
        andq %rdx, %rsi

    you should see (without the `-q` flag, shown with some lines remove for brevity)

        +------------------- between cycles    0 and    1 ----------------------+
        | register cC(N) { SF=0 ZF=1 }                                          |

        +------------------- between cycles    1 and    2 ----------------------+
        | RAX:                0   RCX:                0   RDX:                7 |
        | register cC(S) { SF=0 ZF=1 }                                          |

        +------------------- between cycles    2 and    3 ----------------------+
        | RAX:                0   RCX:                3   RDX:                7 |
        | register cC(S) { SF=0 ZF=1 }                                          |

        +------------------- between cycles    3 and    4 ----------------------+
        | RAX:                0   RCX:                3   RDX:                7 |
        | RBX:                3   RSP:                0   RBP:                0 |
        | register cC(N) { SF=0 ZF=0 }                                          |

        +------------------- between cycles    4 and    5 ----------------------+
        | RAX:                0   RCX: fffffffffffffffc   RDX:                7 |
        | RBX:                3   RSP:                0   RBP:                0 |
        | register cC(N) { SF=1 ZF=0 }                                          |

        +------------------- between cycles    5 and    6 ----------------------+
        | RAX:                0   RCX: fffffffffffffffc   RDX:                7 |
        | RBX:                3   RSP:                0   RBP:                0 |
        | register cC(N) { SF=0 ZF=0 }                                          |

        +------------------- between cycles    6 and    7 ----------------------+
        | RAX:                0   RCX: fffffffffffffffc   RDX: fffffffffffffffb |
        | RBX:                3   RSP:                0   RBP:                0 |
        | register cC(N) { SF=1 ZF=0 }                                          |

        +------------------- between cycles    7 and    8 ----------------------+
        | RAX:                0   RCX: fffffffffffffffc   RDX: fffffffffffffffb |
        | RBX:                3   RSP:                0   RBP:                0 |
        | register cC(N) { SF=0 ZF=1 }                                          |

        +----------------------- halted in state: ------------------------------+
        | RAX:                0   RCX: fffffffffffffffc   RDX: fffffffffffffffb |
        | RBX:                3   RSP:                0   RBP:                0 |
        | register cC(S) { SF=0 ZF=1 }                                          |

2.  You should also now be able to get the same results using your simulator as you get from `tools/yis` when running `y86/prog1.yo` through `y86/prog4.yo` (and `y86/prog8.yo` should still work too).

    We have supplied traces of the output for all the `.yo` files in `testdata/seq-traces`.

## Implementing `cmovXX`

1.  `cmovXX` has the same `icode` as `rrmovq`, but non-zero `ifuns` (least significant nibble of the first byte of the instruction), and will share much of the same logic with `rrmovq`.

2.  We suggest creating a wire called `conditionsMet`, and setting it using a MUX with entries like `ifun == LE : C_SF || C_ZF;` (`C_SF` are the outputs of the condition code registers from above).

3.  Once you have `conditionsMet` wire, in the writeback stage of `cmovXX`, make the `reg_dstE` (or `reg_dstM`, depending on how you implemented `rrmovq`) `REG_NONE` if `conditionsMet` is false.

    Recall that muxes execute only the first true case, so adding something like `!conditionsMet && icode == CMOVXX : REG_NONE;` before other cases when setting the `dst_` should suffice.

    *Remember that `CMOVXX == RRMOVQ`*, so your `conditionsMet` wire should handle `ifun == ALWAYS` (i.e. `ifun == 0`). (`rrmovq` should just become a special case of `cmovXX`.)

## Testing `cmovXX`

If you run your simulator on y86/cmovXX.yo, which is an assembled version of

    irmovq $2766, %rbx  # 0xace → b
    irmovq    $1, %rax  # 1 → a
    andq    %rax, %rax  # set flags based on a (>)
    cmovg   %rbx, %rcx  # move if g  (which it is): b → c  (c now 0xace)
    cmovne  %rbx, %rdx  # move if ne (which it is): b → d  (d now 0xace)
    irmovq   $-1, %rax  # -1 → a
    andq    %rax, %rax  # set flags based on a (<)
    cmovl   %rbx, %rsp  # move if l  (which it is): b → sp  (sp now 0xace)
    cmovle  %rbx, %rbp  # move if le (which it is): b → bp  (bp now 0xace)
    xorq    %rax, %rax  # a ^= a means 0 → a, and sets flags (=)
    cmove   %rbx, %rsi  # move if e  (which it is): b → si  (si now 0xace)
    cmovge  %rbx, %rdi  # move if ge (which it is): b → di  (di now 0xace)
    irmovq $2989, %rbx  # 0xbad → b
    irmovq    $1, %rax  # 1 → a
    andq    %rax, %rax  # set flags based on a (>)
    cmovl   %rbx, %rcx  # move if l  (which it is not): b → c  (not moved)
    cmove   %rbx, %rdx  # move if e  (which it is not): b → d  (not moved)
    irmovq   $-1, %rax  # -1 → a
    andq    %rax, %rax  # set flags based on a (<)
    cmovge  %rbx, %rsp  # move if ge (which it is not): b → sp  (not moved)
    cmovg   %rbx, %rbp  # move if g  (which it is not): b → bp  (not moved)
    xorq    %rax, %rax  # a ^= a means 0 → a, and sets flags (=)
    cmovl   %rbx, %rsi  # move if l  (which it is not): b → si  (not moved)
    cmovne  %rbx, %rdi  # move if ne (which it is not): b → di  (not moved)
    irmovq    $0, %rbx  # 0 → b

you should end with registers

    | RAX:                0   RCX:              ace   RDX:              ace |
    | RBX:                0   RSP:              ace   RBP:              ace |
    | RSI:              ace   RDI:              ace   R8:                 0 |


There is a trace of the expected cycle-by-cycle output in `testdata/seq-traces/cmovXX.txt`.

## Implementing `rmmovq`

1.  The textbook's Figure 4.19 (page 389) notes the following semantics for `rmmovq`:

      -----------------------------------
      Stage     `rmmovq` rA, D(rB)
      --------- -------------------------
      Fetch     icode:ifun ← M~1~\[PC\]\
                rA:rB ← M~1~\[PC + 1\]\
                valC ← M~8~\[PC+2\]\
                valP ← PC + 10

      Decode    valA ← R\[rA\]\
                valB ← R\[rB\]

      Execute   valE ← valB + valC

      Memory    M~8~\[valE\] ← valA

      Writeback  

      PC Update PC ← valP
      -----------------------------------

2.  Memory is accessed by setting `mem_addr` to the memory address in question and either
    -   setting `mem_readbit` to `0`, `mem_writebit` to `1`, and `mem_input` to the value to write to memory, which will cause the memory system to write a 8-byte value to memory; or
    -   setting `mem_readbit` to `1` and `mem_writebit` to `0`, which will cause the memory system to read a 8-byte value from memory into `mem_output`.

3.  You will need to compute the memory address as `reg_outputB` + `valC` (the book suggests you do this in the ALU, meaning the same mux you used for `OPq`'s adding and subtracting).

## Testing `rmmovq`

If both `rmmovq` is implemented correctly, the test case `y86/rmmovq-trivial.yo` should result in address 0xa2 containing byte 0x08. This test case is an assembled version of:

    irmovq $2, %rax
    irmovq $8, %rbx
    rmmovq %rbx, 160(%rax)

## Additional Testing

You can run `make test-seqlab` to test your processor over a variety of files. This will compare the output on the list of `.yo` files in `testdata/seqlab-tests.txt` to the reference outputs in `testdata/seq-reference`.
