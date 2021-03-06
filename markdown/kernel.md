---
title: Kernels -- Software for the Hardware
...


# Kernel Mode vs. User Mode

All multi-purposed chips today have (at least) two modes in which they can operate:
user mode and kernel mode.
Thus far you have probably only written user-mode software, and most of you will never write kernel-mode software during your entire software development careers.

## Motivation

Some activities the computer does can only be done by kernel-mode software;
if user-mode code wants to do them, it has to do so by asking the kernel.
This restriction provides several advantages.

### Limiting Bugs' Potential for Evil

One reason to not run in kernel mode is to limit the scope of mistakes a software bug can have.
Inside the computer is all of the code and data that handles drawing the screen and tracking the mouse and reacting to keys and so on; I'd rather not be able to make a mistake when writing my Hello World program and accidentally mess up those core functionalities.
By running in user mode, if my code tries to touch any of that it will be blocked from doing so: my program will crash, but the computer will keep running.

### Each Program Thinks it is In Control

Physical computers have limited physical resources.
One of the benefits of running in user mode is that the particular limitations are hidden from you by the kernel and hardware working together.
For example, even if you only have 8GB of memory you can still use addresses larger than that limit: the kernel and hardware will work together to provide the illusion that there was enough memory even if there was not.

This illusion is even more important when you are running more than one process simultaneously.
Perhaps you have a terminal, an editor, and your developed code all running at once.
The kernel and hardware will work together to ensure that your code cannot even see, let alone modify, the memory being used by the terminal or editor.
Each program is given the illusion of being the only one running on an unlimited machine.

### Wrapper around Hardware

Each piece of hardware attached to a chip has its own protocols and nuances of operation.
Fortunately, only the kernel needs to know about those details.
If the mouse tries to talk to your program directly, the kernel intercepts the communication and handles it for you so you don't notice it happened unless you asked the kernel to inform you about it.
If you try to talk to the disk, the kernel likewise moderates that communication, keeping you from needing to know the specific protocol the currently-attached disk expects.

### Multiple Users

Because the kernel is an unavoidable intermediary for talking to hardware, it can chose to forbid some interactions.
One common use of this is to allow multiple user accounts.
Since the kernel is the one that controls access to the disk, for example, it can chose not to allow my processes to access the part of the disk it has assigned to you, not me.


## Implementation

### Protected Instructions and Memory

When in user mode, there are a set of instructions that cannot be executed
and segments of memory that cannot be accessed.
Examples of things you cannot directly access in user mode include

- the part of memory that stores information about which user account you have logged into
- instructions that send signals out of the chip to communicate with disks, networks, etc
- instructions that let user-mode code execute in kernel-mode

### Mode Bit

The simplest way to create two modes is to have a single-bit register somewhere on the chip
that indicates if the chip is currently executing in user mode or in kernel mode.
Each protected instruction then checks the contents of that register and, if it indicates user mode, causes an exception instead of executing the instruction.
A special protected instruction is added to change the contents of this mode register,
meaning kernel-mode code can become user-mode, but no vice versa.

Modern processors often have more than two operating modes with several levels of privilege,
which (among other benefits) can help make virtualization efficient;
the details of these additional modes are beyond the scope of this course.

### Mode-switch via Exceptions

The core mechanic for becoming the kernel is a hardware exception.
An exception results in several functionally-simultaneous changes to processor state,
notably including both (a) changing to kernel mode and (b) jumping to code in kernel-only memory.
Thus, the only mechanisms that exist for entering kernel mode will running kernel code for the next instruction, preventing user code from running itself in kernel mode.

The nature of these hardware exceptions and the jump to kernel code that is associated with them is a large enough topic to deserve [it's own section](#exceptions).

# Exceptions

Hardware exceptions are examples of "exceptional control flow":
that is, the sequence of instructions being operated around an exception
is an exception to the usual rules of sequentiality and jumps.
They also tend to appear in exceptional (that is, unusual) situations.

## Kinds

There are several kinds of hardware exceptions.

### Interrupts

An interrupt occurs independently from the code being executed when it occurs.
It might result from the mouse moving, a network packet arriving, or the like.

Interrupts are typically handled by the kernel in a way that is invisible to the user code.
The user code is frozen, the interrupt handled, and then the user code resumed as if nothing had happened.

:::aside
Some sources call all exceptions "interrupts," calling the interrupting-type of exception an "asynchronous interrupt" instead.
:::

### Faults

A fault is the result of an instruction failing to succeed in its execution.
Examples include dividing by zero, dereferencing a null pointer, or attempting to execute a protected instruction while in user mode.

There are two basic families of responses to faults:
either the kernel can fix the problem, then re-run the user code that generated the fault;
or it cannot fix the problem and kills the process that cause the fault instead.
In practice, fixable faults happen quite often, notably the [page fault](#pages) discussed later.
Even faults the kernel can't fix on its own are often handled by asking the user code if it knows how to fix them using a mechanism called [signals](#signals), though many programs do not chose to handle the signals so the end result is often still termination of the fault-generating process.

### Traps

A trap is an exception caused by a special instruction whose sole job is to generate exceptions.
Traps are the main mechanism for intentionally switching from user mode to kernel mode and are the core of all system calls.
System calls are used to interact with the file system, spawn and wait on threads, listen for key presses, and anything else that you cannot do with simple code alone.

## Handling

When an exception occurs, the processor switches to kernel mode and jumps to a special function in kernel code called an "exception handler."
Because interrupts exist, this can happen without any particular instruction causing the jump,
and thus might happen at any point during the code running.
In order for the handler to later resume user code, the exception handling process must also save the processor state before executing the handler.

### Save-Handle-Restore

The basic mechanism for any exception to be handled is

1. Save the current state of the processor (register contents, PC location, etc)
2. Enter kernel mode
3. Jump to code designed to react to the exception in question, called an **exception handler**
4. When the handler finishes, enter user mode and restore processor state (including register contents, PC location, etc)

These steps (except for the actual execution of the exception handler) are all done atomically by the hardware.

### Which Handler?

In general, there can be as many different exception handlers as there are exception causes.
To select which one to run, the hardware consults something called an **exception table**.
The exception table is just an array of code addresses; the index into that array is determined by the kind of exception generated (e.g., divide-by-zero uses index 0, invalid instructions use index 6, etc.)
The index is called an **exception number** or **vector number** and the array of code addresses is called the **exception table**.

:::aside
Switches

Having an array of code addresses is not unique to exception handlers; it is also present in C in the form of a `switch`{.c} statement.

For example, the following C

````c
switch(x) {
    case 0: f0();
    case 1: f1();
    case 3: f3();
    case 2: f2();
        break;
    case 4:
    case 5: f5();
    case 6: f6();
        break;
    case 8: f6();
}
````

compiles down to the following code

````asm
    cmpl    $8, %eax
    ja      AfterSwitch
    jmpq    * Table(,%rax,8)
Case0:
    callq   f0
Case1:
    callq   f1
Case3:
    callq   f3
Case2:
    callq   f2
    jmp     AfterSwitch
Case5:
    callq   f5
Case6:
    callq   f6
    jmp     AfterSwitch
Case8:
    callq   f8
AfterSwitch:
````

supported by the following jump table

````asm
    .section    .rodata
Table:
    .quad   Case0
    .quad   Case1
    .quad   Case2
    .quad   Case3
    .quad   Case5
    .quad   Case5
    .quad   Case6
    .quad   AfterSwitch
    .quad   Case8
````

Exception tables are just one particular use of this array-of-code-addresses idea.
:::

### After the Handler

Handlers may either abort the user code or resume its operation.
Aborting is primarily used when there is no obvious way to recover from the cause of the exception.

When resuming, the handler can either re-run the instruction that was running when it was generated
or continue with the next instruction instead.
A trap, for example, is similar to a `callq` in its behavior
and thus resumes with the subsequent instruction.
A fault handler, on the other hand, is supposed to remove the barrier to success that caused the fault
and thus generally re-runs the faulting instruction.


:::aside
Aborts

We [classified exceptions by cause](#kinds), but some texts classify them by result instead.
If classified by what happens after the handler, there is a fourth class: aborts, which never return.

Fault
:   re-runs triggering instruction

Trap
:   runs instruction after triggering instruction

Interrupt
:   runs each instruction once (has no triggering instruction)

Abort
:   stops running instructions

The abort result may be triggered by any cause:
if a memory access detects memory inconsistency we have an aborting fault;
an exit system call is an aborting trap;
and integral peripherals like RAM can send aborting interrupts if they notice unrecoverable problems.
:::



### Example: Linux system calls

In Linux, almost all communications from user- to kernel-code are handled by a trap with exception number 128.
Instead of using exception number to select the particular action the user is requesting of the kernel, that action is put in a number in `%rax`;
this decision allows Linux to have space for billions of different system calls instead of just the 200 or so that would fit in an exception number and are not reserved by the hardware.

To see a list of Linux system calls, see `man 2 syscall`{.bash}.
Most of these are wrapped in their own library function, listed in `man 2 syscalls`{.bash}

:::example
Consider the C library function `socket`.
The assembly for this function (as compiled into `libc.so`) begins

````asm
socket:
    endbr64 
    mov    $0x29,%eax
    syscall 
````

Let's walk through this a bit at a time:

`endbr64`{.asm}
:   This is part of the [control-flow enforcement](https://software.intel.com/sites/default/files/managed/4d/2a/control-flow-enforcement-technology-preview.pdf) elements of x86-64.
    It has no outwardly visible impact on program functionality,
    but it does add some security enforcement,
    making it harder for some classes of malicious code
    to access this function.
    
    Omitting some (important but tedious to describe) details,
    because this instruction is present in the function
    you can't jump into the code except at that line.
    That means malicious code can't jump a bit later into this function
    with the goal of executing the `syscall` instruction
    without first going through the `%rax`-setting instruction.

`mov $0x29,%eax`{.asm}
:   Places 29~16~ (41~10~) into `%rax`. 
    Per `/usr/include/sys/syscal.h`, 41 is the number of the "socket" syscall.

`syscall`{.asm}
:   A trap, generating exception number 128.
    This means the hardware saves processor state,
    then jumps to the address stored in the kernel's `exception_handler[128]`.
    
    The exception handler at index 128 checks that `%rax` contains a valid system call number (which 41 is),
    after which it jumps to the kernel's `system_call_handler[41]`, the address of the socket call implementation.
    
    The kernel's system calling convention has the same first three arguments as C's calling convention,
    so the handler has access to the arguments from the `int socket(int domain, int type, int protocol)`{.c} invocation.
    It uses them to do whatever work is needed to create a socket,
    placing its file descriptor in `%rax` to be a return value.
    
    The handler ends by executing a protected return-from-handler instruction
    that 
    
    - restores processor state (but leaves `%rax` alone as a return value),
    - returns to user mode, and
    - jumps back to user code
    
After that is some error checking code, and then the function returns. The whole function is only 11 instructions (24 bytes) long.
The code in `system_call_handler[41]` of the kernel is considerably longer; many thousands of lines of C code, in fact (see <https://github.com/torvalds/linux/tree/master/net>).
:::

## Exception-Like Constructs

### Signals

One view of exceptions is that they enable communication from user code to the kernel.
Signals permit the opposite direction of communication.

                     ??? User code     ??? Kernel code   ??? Hardware
------------------- --------------  --------------- -----------------------
**User code ???**     ordinary code   Trap            _via kernel_
**Kernel code ???**   **Signal**      ordinary code   protected instructions
**Hardware ???**      _via kernel_    Interrupt       ---

Signals are roughly the kernel-to-user equivalent of an interrupt.
At any time, while executing any line of code,
a signal may appear.
It will suspend the currently executing code and see if you've told the kernel
about a signal handler, a special function you believe will handle that signal.
After running the handler, the interrupted code will resume.

Linux defines many different signals, listed in `man 7 signal`{.bash}.
Each of them has a default action if no handler is registered,
most commonly terminating the process.

:::example
Typing Ctrl+C on the command line causes the SIGINT signal to be generated.
If we want Ctrl+C to do something else, we have to handle that signal:

````c
#include <stdio.h>
#include <signal.h>

static void handler(int signum) {
    printf("Provide end-of-file to end program.\n");
}

int main() {
    struct sigaction sa;       // how we tell the OS what we want to handle
    sa.sa_handler = handler;   // the function pointer to invoke
    sigemptyset(&sa.sa_mask);  // we'll use sigaction, not signal, so set not needed
    sa.sa_flags = SA_RESTART;  // restart functions that can recognize they were interrupted

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        fprintf(stderr, "unable to override ^C signal");
        return 1;
    }

    char buf[1024];
    while(scanf("%1023s", buf) && !feof(stdin)) {
        printf("token: %s\n", buf);
    }
}
````
:::

:::example
Almost all signals can be overridden, but for many it is not wise to do so.
For example, this code:

````c
#include <stdio.h>
#include <signal.h>

static void handler(int signum) {
    printf("Ignoring segfault.\n");
}

int main() {
    struct sigaction sa;       // how we tell the OS what we want to handle
    sa.sa_handler = handler;   // the function pointer to invoke
    sigemptyset(&sa.sa_mask);  // we'll use sigaction, not signal, so set not needed
    sa.sa_flags = SA_RESTART;  // restart functions that can recognize they were interrupted

    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        fprintf(stderr, "unable to override segfault signal");
        return 1;
    }

    // let's make a segfault -- enters infinite handler loop
    char * buf;
    buf[1234567] = 89;
}
````

... will print "Ignoring segfault" repeatedly until you kill it with something like Ctrl+C.
The last line creates a segfault, which causes the OS to run the handler.
The handler returns normally, so the OS assumes the cause of the fault was fixed and re-runs the triggering code,
which generates another segfault.

Obviously, you should not do something like this in real code.
:::

There is a system call named "kill" that asks the kernel to send a signal to a different process.
While this can be used for inter-process communication,
better systems (like sockets and pipes) exist for that if it is to be a major part of an application's design.

:::aside
Command line and signals

From a bash command line, you can send signals to running processes.

- Ctrl+C sends SIGINT, the "interrupt" signal that means "I want you to stop doing what you are doing."
- Ctrl+Z sends SIGSTOP, a signal that cannot be handled. It always causes the OS to suspend the process, freezing it in place until you ask for it to resume.
- `bg` causes a suspended process to resume running, but in the background so you can use the terminal for other purposes.
- `fg` causes a suspended or background process to resume running in the foreground, re-attaching the keyboard to `stdin`.
- `kill <pid>` sends SIGTERM, the "terminate" signal, to the process with process ID `<pid>`.
    The "terminate" signal means "I want you to shut down now" but can be handled to, e.g. save unsaved work or the like.
- `kill -9 <pid>` sends SIGKILL, the "kill" signal, to the process with process ID `<pid>`.
    The "kill" signal cannot be handled; it always causes the OS to terminate the program.
    
    Any other signal can be provided in similar fashion, either by number (SIGKILL is signal number 9)
    or name (e.g., `kill -KILL <pid>`)

There are other tools for sending signals, but the above are sufficient for most common use cases.
:::

### `setjmp`{.c}, `longjmp`{.c}, and software exceptions

Portions of the save- and restore-state functionality used by exception handlers
is exposed by the C library functions `setjmp` and `longjmp`.

`setjump`, given a `jmp_buf` as an argument, fills that structure with information needed to resume execution at that location and then returns 0.
Thereafter `longjmp` can be called with that same `jmp_buf` as an argument;
`longjmp` never returns, instead "returning" from `setjmp` for a second time.
`longjmp` also provides an alternative return value for `setjmp`.

:::example
The following program

```c
#include <setjmp.h>
#include <stdio.h>

jmp_buf env;
int n = 0;

void b() {
    printf("b1: n = %d\n", n);
    n += 1;
    printf("b2: n = %d\n", n);
    longjmp(env, 1);
    printf("b3: n = %d\n", n);
    n += 1;
    printf("b4: n = %d\n", n);
}

void a() {
    printf("a1: n = %d\n", n);
    n += 1;
    printf("a2: n = %d\n", n);
    b();
    printf("a3: n = %d\n", n);
    n += 1;
    printf("a4: n = %d\n", n);
}

int main() {
    printf("main1: n = %d\n", n);
    n += 1;
    printf("main2: n = %d\n", n);
    int got = setjmp(env);
    if (got) {
        printf("main3: n = %d\n", n);
        n += 1;
        printf("main4: n = %d\n", n);
    } else {
        printf("main5: n = %d\n", n);
        n += 1;
        printf("main6: n = %d\n", n);
        a();
    }
    printf("end of main\n");
}
```

prints

    main1: n = 0
    main2: n = 1
    main5: n = 1
    main6: n = 2
    a1: n = 2
    a2: n = 3
    b1: n = 3
    b2: n = 4
    main3: n = 4
    main4: n = 5
    end of main

See also a [step-by-step simulation](longjmp.html) of this same process without the `printf`s.
:::

There was a time when `setjmp`/`longjmp` were seen as effective ways of achieving
`try`/`catch`-like error recovery,
which cannot be handled with simple `goto` because it may skip multiple function call/returns.

+---------------------------------------+---------------------------------------+
| `try`/`catch` constructs              | `setjmp`/`longjmp` parallel           |
+=======================================+=======================================+
| ````java                              | ````c                                 |
| try {                                 | if (!setjmp(env)) {                   |
|     f();                              |     f();                              |
| } catch {                             | } else {                              |
|     g();                              |     g();                              |
| }                                     | }                                     |
| ````                                  | ````                                  |
+---------------------------------------+---------------------------------------+
| `throw new Exception3();`{.java}      | `longjmp(env, 3)`{.c}                 |
+---------------------------------------+---------------------------------------+

More efficient approaches to this have since been developed.
`setjmp` records much of the state of the program in advance,
which gives it a significant cost in time;
because `try` is generally assumed to be executed far more often than `throw`,
we'd rather make `throw` the expensive one, not `try`.
Most of the information stored by `setjmp` into the `jmp_buf`
can be found somewhere in the `setjmp`-invoking function's stack frame,
which many languages maintain with sufficient discipline
that it can be "unwound" to restore a previous state upon a `throw`.
C, however, lets you do anything, including violating assumptions about the stack organization,
so C has an expensive `setjmp` and less expensive `longjmp`
instead of an expensive `throw` and less expensive `try`.


