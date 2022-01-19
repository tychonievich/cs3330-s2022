---
title: Get C
...

This lab is ungraded and unstaffed. In theory you've already done all of this in a previous course.
Many subsequent assignments depend on this lab.

# Overview

For some labs this semester, you'll need

- Linux
- An x86-64 processor
- A C compiler
- The Unix utility `make`
- Python 3

Not all assignments will need all of these components, but some will require each.

# The Officially Supported Solution

You may do your assignments on the department Linux machines. 

:::aside
If you had a department account last semester, you should still have a department account this semester. For others students, accounts will be created by the second week of class; see your email for the initial password. (Look for an email sent from cshelpdesk and, for most students, the week before classes started.)
:::

:::aside
To reset your password, use the password reset tool at <https://www.cs.virginia.edu/PasswordReset/>. (This tool will not work unless your account was setup (at some point) to have its initial temporary password changed.)
:::

## Accessing the Server

Two ways of using these machines are supported:

1. Use the NoMachine remote desktop and the NX cluster, composing code directly on the servers. See [the department's documentation](https://www.cs.virginia.edu/wiki/doku.php?id=nx_lab) for more.

2. Use [SSH](//www.cs.virginia.edu/luther/tips/ssh.html), with either [Git](//www.cs.virginia.edu/luther/tips/git.html), [SFTP, or SCP](//www.cs.virginia.edu/luther/tips/ssh.html#copying-files) to move files to and from the servers.

    <div class="aside">Warning: Do not submit files by copy-paste from ssh terminals without looking at them first! Copy-paste can introduce line breaks, backslashes, and so on where they do not belong.</div>

## Using Modules

Either way, you'll end up running on one of the department's shared Linux clusters.
Because it's shared with many users, these machines use [modules](http://modules.sourceforge.net/) to let different users have access to different software and versions of software.

In most cases, this means you'll want to start each session by calling

```bash
module load clang-llvm gcc git python3-3.9.9
```

There are other packages available, including a few non-`vi` command-line editors like `emacs` and `nano`; see `module avail` for a list.

:::aside
As of 2022-01-18, module `clang-llvm` is clang version 6.0.
Clang version 14 can be loaded using `module load WIP-clang-llvm-14.0.0`;
the changes to clang's build process have made our systems staff less than fully confident that version 14 was completely properly installed, hence the WIP (Work In Progress) tag.
I tried it and it worked for me, with the updated performance and nicer error messages that version 14 offers over version 6.
That last message I had from our systems staff said

> Once your students have confirmed its functionality, I’ll remove the “WIP”.

so if you use the version 14 module and it works (or doesn't), please let me know.
:::

# Alternative paths

1. Go native. Install a Linux distro (there are many; perhaps try a [popular](https://ubuntu.com/), [stable](https://www.centos.org/), or [advanced](https://archlinux.org/) version)

2. Use virtualization, such as virtualbox. Note, you’ll need a 64-bit image of Linux, like the one 2150 has used recently. Virtualization occasionally messes up timing, so it might not be good for the last part of the course, but it should work until then.

3. Use an online IDE. There are many web-based IDEs that run an appropriate version of Linux under the hood.
    [Cloud9](https://aws.amazon.com/cloud9/), [Codio](https://www.codio.com/), [ideone](https://ideone.com/), and [ShiftEdit](https://shiftedit.net/) all might work.

4. Use your OS's POSIX-like interface.
    
    MacOS is based on Darwin, which is enough like Linux for most assignments (but not the Bomb assignment).
    
    Windows has the [Windows Subsystem for Linux](https://docs.microsoft.com/en-us/windows/wsl/about), which is theoretically fully Linux-compatible, but we've not tested that theory.

# Testing your environment

## C command line

C files can be compiled on any Linux system using `gcc -x c filename.c`, `clang -x c filename.c`, or `llvm-gcc -x c filename.c`. Most systems will have only one of these three installed; it does not matter which one you use.

If you have no `main` method or otherwise want to produce an object file instead of a final executable, add `-c` to the command line.

## 2150's Unix Tutorial

You might refer to [CS 2150's Unix tutorial](https://uva-cs.github.io/pdr/tutorials/01-intro-unix/index.html) for information about how to use Unix. Alternatively, we provide an extremely brief introduction below.

This tutorial includes instructions for using the native Unix enviornment on OS X in addition to more Linux-like environments. As noted above, we need a x86-64 Linux environment for certain assignments, including the first one (bomb lab/HW). So, although the native OS X environment may be most convenient for future assignments, if you are using OS X, we recommend following the tutorial [on the department machines](#the-officially-supported-solution).

## Linux Lite

We assume you'll use Linux for the labs in this course.

Upon logging into Linux, assuming you're logging in via graphical interface (such as through NX), you'll want access to

1.  A terminal window
2.  An editor of some kind
3.  A web browser

I suggest getting the terminal first by pressing Alt-F2 and typing `gnome-terminal`, `konsole`, or `xterm` (they may not all work, but at least one should).

You can then get an editor by typing into the terminal one of `geany &`, `gedit &`, `kate &`, `nano`, `pico`, `emacs`, or `vim` (or others, if you know others); and you can get a browser with `firefox &` or `chromium-browser &`. The `&` means "Run this in the background and let me type other stuff in the terminal while it is running".

Other important commands you can use in the terminal:

-   `pwd` tells you where you are currently in the file system
-   `ls` tells you what files are in the current folder
-   `mkdir` makes a new directory
-   `cd ..` go one spot higher in the directory tree (if you are in `/home/mst3k/funbox/whee/` then `cd ..` will move you to `/home/mst3k/funbox/`)
-   `cd dirname` to enter directory dirname (if you are in `/home/mst3k/funbox/` then `cd whee` will move you to `/home/mst3k/funbox/whee/`)
-   The Tab key will autocomplete what you are typing; pressing it twice will list all possible autocompletions. For example, instead of typing `/home/mst3k/funbox/whee/` I can probably type `/h`Tab`m`Tab`fu`Tab`wh`Tab. This not only saves typing, it reduces the chances of typos.

### Linux Lite with SSH

If you login into Linux via `ssh`, you won't have to do any work to get a terminal, but that's all you'll have.

You won't be able to (usefully) run a web browser on the remote machine; instead, you'll probably using a web browser on your local (non-Linux) machine and transferring files using our instructions on SCP.

However, you'll still be able to take advantage of the above advice regarding using the terminal and finding text editors (though you won't be able to use any of the text editors you'd run in the background).

# Testing your compiler

If you think you have a compiler properly installed,

1.  create `hello.c` from Figure 1.1 on page 2 of the textbook (or [here](http://csapp.cs.cmu.edu/3e/ics3/code/intro/hello.c))
2.  compile it as described in §1.2, and
3.  run it as described in §1.4



