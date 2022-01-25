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
        
        ````bash
        ME=mst3k; curl "http://kytos.cs.virginia.edu:15215/?username=$ME&submit=Submit" > bomb.tar
        ````
        
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
