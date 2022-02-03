---
title: Bit fiddling Lab and HW
...

> Due to server problems on the day of the lab, we have made the following changes:
>
> - The lab due-date has been pushed back to the same time as the homework due-date
> - You may still collaborate on the lab tasks, but not on the homework tasks
> - For full credit, you only need to do the 4 lab tasks and any 3 of the 4 homework tasks

This will help you get used to using bitwise operators to do more complicated tasks.
Some of these are designed to help you reinforce your understanding of binary operations,
while others are techniques you'll likely find useful in several subsequent assignments.

# Lab

1. Visit <https://kytos.cs.virginia.edu/cs3330/bits.php>
2. In pairs or small groups, collaborate on solutions to
    a. subtract
    b. bottom
    c. endian
    d. bitcount

# Homework

**on your own**, solve at least three of

e. getbits
f. anybit
g. reverse
h. fiveeights

# Hints

If needed, we have some hints you can look at.

<style>
summary { font-weight: bold; }
details { padding: 1ex; margin: 1ex 0ex; }
details[open] { border-left: thin solid rgba(0,0,0,0.25); border-radius:1ex; }
</style>


<details><summary>subtract</summary>

Consider the definition of two's compliment.

</details>


<details><summary>bottom</summary>

The obvious solution `~(0xFFFFFFFF << b)`{.c} won't work.
Bit shifts always do a modulo on their right-hand operand, so `a << b` does the same thing as `a << (b % (8*sizeof(a))`.
Thus, `<< 32` and `<< 0` do the same thing.

</details>


<details><summary>endian</summary>

Remember that changing endianness means swapping bytes, not bits.

</details>


<details><summary>bitcount</summary>

The obvious solution would be something like

````c
ans = 0;
for(int i=0; i<32; i+=1) {
    a += x&1;
    x >>=1;
}    
````

We don't allow for loops, but even if you replace it with 32 copies that's still 96 operations, and we only allow 40 for this task.

The trick is to do things in parallel, treating a number like a vector of smaller numbers.
Suppose I wanted to count the bits of an 8-bit number with bits `abcdefgh`.
With a little shifting and masking I could make three numbers

    0b00e00h
    0a00d00g
    0000c00f

and add them to get `xx0yy0zz` where `xx = a+b`, `yy = c+d+e`, and `zz = f+g+h`.

Extending this trick to several rounds on 32 bits will solve this problem.

</details>





<details><summary>getbits</summary>

Your solution to `bottom` plus a little shifting should do the trick.

</details>


<details><summary>anybit</summary>

The easy solution would be `y = !!x` but we don't allow `!`. Nor do we allow enough operations to do a loop-like solution.
    
You can divide and conquer.
Try defining `x1` where if any bit anywhere in `x` was `1`, some bit in the bottom 16 bits of `x1` is `1`.
The given task is "see if any `1` bit is in the 32 bits of `x`".
How could you reduce it to "see if any `1` bit is in the bottom 16 bits of `x1`"?

</details>


<details><summary>reverse</summary>

There aren't enough operations to do the endian-like reversal.
But you can use a vector-like solution: suppose you had an endian-reversed version of `x`: how could you reverse the bits of all four bytes at the same time?

</details>


<details><summary>fiveeighths</summary>

Any solution that includes left-shifts of adding a number to itself won't work for large inputs.
Think in terms of right shifts and additions.

<details><summary>more...</summary>

A solution for positive numbers won't work for all negative numbers. To see this, try writing out cases `-8` to `8` and their solutions in binary.

If you can solve this for positive numbers, what would you need to change to solve it for negatives too?

</details>
</details>

