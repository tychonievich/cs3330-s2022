---
title: Out-of-order Processors
...

There are a number of interrelated ideas in out-of-order execution, and multiple implementations of each part.
The goal of this section is to explain enough that you can see how the pieces might be built, not to explain the design decisions needed to pick the best build.

# Functional units and issue queues

Suppose our computer has an adder circuit, used by multiple instructions (e.g., to compute addresses and to perform `addq`, `subq`, and `cmpq`, instructions, etc.)
We're going to call this adder a **functional unit**,
give it an explicit set of operands (i.e., $x = y + z$ not $x += y$),
and give it its own little **issue queue** (or **reservation station**) of additions waiting to happen.

The queue is going to be a fixed set of registers.
These will specify what values to work on and where to put the result.
However, since we might have dependencies, there will be a special **ready bit**
for each operand; an instruction will be ready only if all of its operands' ready bits are 1.
Setting ready bits is discussed in the [Common data bus] section.

Each cycle the adder will look at the slots in its queue and execute the oldest instruction that is ready to be executed,
broadcasting the results onto the [common data bus].
If there is no such instruction, it will do nothing.

When the issue stage wants to put an instruction into the adder's issue queue,
it will first make sure there's an open slot;
if not, the issue stage will stall until the adder has room for the next instruction.

There's nothing special about an adder here; we'll also have other functional units, like a multiplier, a memory reader, a memory writer, etc.

We might have multiple functional units that do the same work, relying on the issue stage to send tasks to one that will get to it quickly.
We also might have a functional unit that can handle multiple tasks in a single cycle, either by having its own half-cycle clock or by having multiple parallel copies of its logic gates, relying on it to handle grabbing several items from the queue at a time.
We also might have a functional unit that takes a long time to finish a task, leaving it in the issue queue for many cycles while it churns away
before finally broadcasting a result and removing it from the queue.

# Register renaming

Our functional units will be much simpler if we have a simplified version of data dependencies.
We don't want to ever have a case where several pending instructions are trying to write to the same register and we get the results our of order.
So we'll do a special renaming process, changing program registers like `%rax` into secret hardware registers we actually use.

The main hardware data structure we'll need for this is a **remap file**; this is an array of entries, one per program register, each entry of which contains two fields: a **tag** which is a hardware register number and a **ready bit** which tells us if some pending instruction will overwrite this (`0`, not ready) or not (`1`, ready).

:::example
The x86-64 registers RAX through R15 are internally register numbers 0 through 15, so we'll need a 16-entry remap file.
If we have 64 hardware registers and no pending instructions, this file might look like

 index       ready   tag
---------   ------- --------------
0 = RAX     1       0
1 = RCX     1       11
2 = RDX     1       9
3 = RBX     1       15
⋮           ⋮       ⋮
14 = R14    1       38
15 = R15    1       2

This means that, e.g., the program register value for `%rcx` is currently stored in hardware register number 11.
:::

Every time we issue an instruction, we'll change its source operands into the tags from the file, then allocate a new unused tag for its destination operand and update the remap file to have that new tag and to be marked as not ready.

:::example
Continuing the example from above,
assume that hardware registers 18 and 19 are unused right now (not in the rename file)

We'd change the instruction `addq %rax, %rcx`
into `h18 = h0* + h11*`
where `h18` means "hardware register 18"
and `*` means "is ready", and update the remap file

 index       ready   tag
---------   ------- --------------
0 = RAX     1       0
1 = RCX     0       18
2 = RDX     1       9
3 = RBX     1       15
⋮           ⋮       ⋮

If the instruction after that was `addq %rcx, %rdx`
it would become
into `h19 = h18 + h9*`

 index       ready   tag
---------   ------- --------------
0 = RAX     1       0
1 = RCX     0       18
2 = RDX     0       19
3 = RBX     1       15
⋮           ⋮       ⋮

and so on
:::

This process ensures that each hardware register has at most one pending instruction that is writing to it, while still maintaining the dependency graph of the original instructions.

# Reorder buffer

Because we might encounter an exception or discard work due to a misprediction at any time, we want to make sure that we never see any externally-visible results of an instruction before an earlier instruction has completed.
To solve this, we'll have a special queue of incomplete instructions called a **reorder buffer**.

When an instruction is issued, it is also enqueued into the reorder buffer.

When an instruction completes, we

- put its result into its hardware register and mark it as ready in the remap file
- mark it as done in the reorder buffer
- if it was the oldest undone instruction in the reorder buffer before, move its results over into the externally-visible state (set flags, update the register file, perform memory writes, etc)

# Common data bus

When an instruction completes, we might need to do any or all of the following:

- update the hardware register with the result
- update the remap file to show the register is ready
- update all pending instructions that used the destination as a source with the newly-available result, marking those operands as ready
- update the reorder buffer to show the instruction is done, possibly also setting externally-visible state

This is potentially a lot of work, and rather than trying to have one system do it all we have an internal **bus** that we broadcast results on.
There are many kinds of buses, but the basic idea is that any connected system can broadcast a message on the bus and all other connected systems receive it.
The messages for this bus are simple: "hardware register 23 is now 0x123456";
each functional unit, the remap file, and the reorder buffer listen to the bus and adjust things as needed.


