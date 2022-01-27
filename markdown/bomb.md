---
title: Bomb Lab and HW
...

A Mad Programmer got really mad and created a slew of "binary bombs".
Each binary **bomb** is a program, running a sequence of phases.
Each **phase** expects you to type a particular string.
If you type the correct string, then the phase is defused and the bomb proceeds to the next phase.
Otherwise, the bomb **explodes** by printing "BOOM!!!", **telling us it did so**,
and then terminating.

# Lab

1. Each student should download their own bomb from <http://kytos.cs.virginia.edu:15215/>
    
    - Either interactively from a browser
    
    - Or on the command line as
        
        `````bash
        ME=mst3k; curl "http://kytos.cs.virginia.edu:15215/?username=$ME&submit=Submit" > bomb.tar
        `````
        
    For credit, you must use your Computing ID with lower-case letters.
        
        
2. In pairs or small groups, figure out how to prevent the bomb from exploding even if you don't know all the answers.
    
    You get 10 free explosions; after that we start taking off points

3. In pairs or small groups *carefully* help one another diffuse phases 1 and 2
    
    Note: every phase 1 is similar in design (though not in solution) but phase 2s designs differ from one bomb to the next.

# Homework

**On your own**, diffuse at least phase 3.

Phases diffuses     Grade           Notes
-----------------   ---------       -----------
Phase 1             Lab 85%         May work with others in lab
Phase 2             Lab 100%        May work with others in lab
Phase 3             HW 90%          Do on your own
Phase 4             HW 100%         Do on your own
More phases         +1% each        Do on your own; extra credit

The first 10 explosions are free. After that, $-1$ per explosion.

# Tips

1. Extract the bomb using `tar -xvf bomb#.tar`{.bash} where `#` is your bomb number.
    
    If you downloaded the bomb with `curl` instead of a browser, do `tar -xvf bomb.tar`{.bash} instead: curl saves the tarball without the number in the filename.
2. `cd bomb#` (again, where `#` is your bomb number).
3. Read the `README`
4. You are welcome to look at `bomb.c` -- it isn't very interesting, though
5. Do whatever you need to to understand what the bomb is doing
6. Only run the bomb `./bomb` once you are confident you can defuse a phase (or at least avoid an explosion)
7. Once you pass a phase visit [the scoreboard](http://kytos.cs.virginia.edu:15215/scoreboard) to verify that we saw your success.

## Hints

If you run your bomb with a command line argument, for example, `./bomb psol.txt`, then it will read the input lines from `psol.txt` until it reaches EOF (end of file), and then switch over to the command line. This will keep you from having re-type solutions.

Because you want to avoid explosions, you'll want to set a breakpoint *before* you run the program so that you can stop the program before it gets to a the function that does the exploding.

You might find it useful to run, `objdump --syms bomb` to get a list of all symbols in the bomb file, including all function names, as a starting point on where you want your breakpoint.

Once you hit a breakpoint in GDB or LLDB, `disassemble` will show you the assembly near the breakpoint and `up` step out of the current function into the calling function so you can `disassemble` it too.

Each phase is doing a different kind of algorithm under the hood, ranging from simple string comparison to recursive data structure traversal.



## Basic Strategy

-   The best way is to use your favorite debugger to step through the disassembled binary. **Almost no students succeed without using a debugger like gdb or lldb.** On the department Unix machines, `gdb` is the debugger that is available. By default GDB verison 7.3 is available, and you can use GDB version 8 by running `module load gdb-8.1` then running `gdb`. You will need to run this `module load` command in each new terminal (the setting will not persist).

-   Try running `objdump -t bomb`. This will show you the symbols in the executable, including the names of all methods. Look for one that looks dangerous, as well as some that looks like interesting methods (perhaps something like "Phase 1").

-   To avoid accidentally detonating the bomb, you will need to learn how to single-step through the assembly code and how to set breakpoints. You will also need to learn how to inspect both the registers and the memory states.

-   It may be helpful to use various utilities for examining the bomb program outside a debugger, as described in "examining the executable" below.

## Bomb Usage

-   The bomb ignores blank input lines.

-   If you run your bomb with a command line argument, for example,

    ````
     linux> ./bomb psol.txt
    ````

    then it will read the input lines from `psol.txt` until it reaches EOF (end of file), and then switch over to `stdin`. This will keep you from having re-type solutions.

## Examining the Executable

-   `objdump -t` will print out the bomb's symbol table. The symbol table includes the names of all functions and global variables in the bomb, the names of all the functions the bomb calls, and their addresses. You may learn something by looking at the function names!

-   `objdump -d` will disassemble all of the code in the bomb. You can also just look at individual functions. Reading the assembler code can tell you how the bomb works.

    If you prefer to get Intel syntax disassembly from `objdump`, you can use `objdump -M intel -d`.

-   `strings` is a utility which will display the printable strings in your bomb.

## Using GDB

-   If you are on a department Unix machine, `module load gdb-8.1` first (this needs to be done once per terminal), so `gdb` is the most recent available version of GDB. (By default you will get GDB 7.3.)

-   Run bomb from a debugger like gdb instead of running it directly. The debugger will allow you to stop the bomb before it detonates.

    For example, if I ran

    ````
    linux> gdb bomb
    (gdb) b methodName
    (gdb) run
    (gdb) kill
    ````

    this will start `gdb`, set a **breakpoint** at `methodName`, and run the code. The code will halt *before* it runs `methodName`; calling `kill` will stop the bomb and exit the current debugging session without `methodName` running.

-   Use "step" and "stepi" to examine this function. "step" runs your code one line of **source code** at a time. "stepi" runs your code one line of **machine instruction** at a time. This allows you to run "phase_1()" piece by piece.

-   Use this to step *carefully* through `phase_1()` to see if you can find the passphrase.

    ````
    linux> gdb bomb
    (gdb) b lineNumberForPhase1Call
    (gdb) run
    ````

    *input test passphrase here*

    ````
    (gdb) stepi
    (gdb) info locals
    (gdb) info registers
    ````

    *Generally some parameters are local variables and some are stored in registers. You should see your test passphrase here. Note the hex value of your input*

    ````
    (gdb) stepi
    ````

    *If you want to see the assembly code you're stepping through, use "disas methodNmae".*

    *keep `stepi`ing until you see `strings_not_equal` method (a suspicious name that might be checking your passphrase)*

    ````
    (gdb) info locals
    (gdb) info registers
    ````

    *Which one holds your passphase? Try "examining" that and others...*

-   Some useful `gdb` commands:

    `(gdb) info locals`
    :   prints out the name and value of local variables in scope at your current place in the code.

    `(gdb) info registers`
    :   prints the values of all registers except floating-point and vector registers

    `(gdb) x/20bx 0x...`
    :   examine the values of the 20 bytes of memory stored at the specified memory address (0x...). Displays it in hexadecimal bytes.

    `(gdb) x/20bd 0x...`
    :   examine the values of the 20 bytes of memory stored at the specified memory address (0x...). Displays it in decimal bytes.

    `(gdb) x/gx 0x...`
    :   examine the value of the 8-byte integer stored at the specified memory address.

    `(gdb) x/s 0x...`
    :   examines the value stored at the specified memory address. Displays the value as a string.

    `(gdb) x/s $someRegister`
    :   examines the value at register someRegister. Displays the value as a string (assuming the register contains a pointer).

    `(gdb) print expr`
    :   evaluates and prints the value of the given expression

    `call (void) puts (0x...)`
    :   calls the built-in output method `puts` with the given `char *` (as a memory address). See `man puts` for more.

    `(gdb) disas methodName`
    :   gives you the to get the machine instruction translation of the method `methodName`. (If you see a `call` instruction in this output that does not indicate what function it refers to, if on the department machines, make sure you are using gdb version 8.1.)

    `(gdb) disas`
    :   gives you the to get the machine instruction translation of the currently executing method.

    `(gdb) x/6i 0x...`
    :   try to disassemble 6 instructions in memory starting at the memory address 0x...

    `(gdb) set disassembly-flavor intel`
    :   switches GDB to Intel syntax disassembly (the syntax you used in 2150; not the syntax we will use for the rest of the course)

    `(gdb) set disassembly-flavor att`
    :   switches GDB back to AT&T syntax disassembly, the default and the syntax we will use for the rest of the course

    `(gdb) b *0x...`
    :   set a breakpoint at the specified memory address (0x...).

    `(gdb) b function_name.`
    :   set a breakpoint at the beginning of the specified function.

    `(gdb) nexti`
    :   step forward by one instruction, skipping any called function.

    `(gdb) kill`
    :   termiante the program immediately

    `(gdb) help`
    :   brings up gdb's built-in help menu

    The textbook also has a nice summary of useful gdb commands on page 280 (or 255 of the 2nd edition). You can also find sources like [this one from the textbook authors](http://csapp.cs.cmu.edu/2e/docs/gdbnotes-x86-64.txt) that list and describe other useful gdb commands.

## On interpreting the disassembly

-   Reviewing the x86-64 calling convention (Figure 3.28 in the textbook or [this reference you may remember from 2150](https://aaronbloomfield.github.io/pdr/book/x86-64bit-ccc-chapter.pdf)) may be helpful.

-   The C standard library function `sscanf` is called `__isoc99_sscanf` in the executable. Try `man sscanf` for more information about this library function.

-   `%fs:0x1234` refers to a value in a "[thread-local storage](https://en.wikipedia.org/wiki/Thread-local_storage)" region at offset `0x1234`. The bomb only has one thread (using multiple threads would allow the bomb to do multiple things at once, but that is not something the bomb needs), so this is effectively a region for extra global variables. In the bomb, this appears mostly to implement [stack canaries](https://en.wikipedia.org/wiki/Stack_buffer_overflow#Stack_canaries), a security feature designed to cause out-of-bounds accesses to arrays on the stack to more consistently trigger a crash.

-   Pay attention to the names of functions being called.

-   Disassembling a standard library function instead of reading the documentation for the function is probably a waste of time.

-   Some of the things later phases might be using include:

    -   calls to `scanf` (which is much like a backwards version of `printf`; try `man scanf` for more)
    -   linked data structure traversal
    -   recursion
    -   string literals
    -   `switch` statements

