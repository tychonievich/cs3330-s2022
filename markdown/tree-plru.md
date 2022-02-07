---
title: Tree-PLRU
...

An $n$-way true least-recently-used (LRU) cache would require $\lceil\log_2(n!)\rceil$ bits of state and more logic than we wish to handle for a fast cache.
Many caches instead use a pseudo-LRU (PLRU) system with state that requires fewer gates to access and update.
This page describes one such PLRU: the Tree-PLRU.

# Full Binary Tree of Bits

The state stored by a PLRU is a full binary tree of bits, with lines as the leaf nodes.
The bits indicate which subtree has the PLRU leaf:
0 for the left-hand tree, 1 for the right-hand tree.
Thus, in this tree:

<center>
<svg viewBox="-38 -30 156 73" style="width:15.6em">
<g style="font-size:8pt;text-anchor:middle">
<text x="40" y="-20">0</text>
<text x="0" y="0">1</text>
<text x="80" y="0">1</text>
<text x="-20" y="20">0</text>
<text x="20" y="20">0</text>
<text x="60" y="20">1</text>
<text x="100" y="20">0</text>
<!-- -->
<text x="-30" y="40">0</text>
<text x="-10" y="40">1</text>
<text x="10" y="40">2</text>
<text x="30" y="40">3</text>
<text x="50" y="40">4</text>
<text x="70" y="40">5</text>
<text x="90" y="40">6</text>
<text x="110" y="40">7</text>
</g>
<g style="stroke:black;fill:none">
<line x1="35" x2="5" y1="-18" y2="-10"/>
<line x1="45" x2="75" y1="-18" y2="-10"/>
<line x1="-5" x2="-15" y1="2" y2="10"/>
<line x1="5" x2="15" y1="2" y2="10"/>
<line x1="75" x2="65" y1="2" y2="10"/>
<line x1="85" x2="95" y1="2" y2="10"/>
<line x1="-23" x2="-27" y1="22" y2="30"/>
<line x1="-17" x2="-13" y1="22" y2="30"/>
<line x1="17" x2="13" y1="22" y2="30"/>
<line x1="23" x2="27" y1="22" y2="30"/>
<line x1="57" x2="53" y1="22" y2="30"/>
<line x1="63" x2="67" y1="22" y2="30"/>
<line x1="97" x2="93" y1="22" y2="30"/>
<line x1="103" x2="107" y1="22" y2="30"/>
<rect x="-36" y="30" width="12" height="12"/>
<rect x="-16" y="30" width="12" height="12"/>
<rect x="4" y="30" width="12" height="12"/>
<rect x="24" y="30" width="12" height="12"/>
<rect x="44" y="30" width="12" height="12"/>
<rect x="64" y="30" width="12" height="12"/>
<rect x="84" y="30" width="12" height="12"/>
<rect x="104" y="30" width="12" height="12"/>
</g>
</svg>
</center>

Line number 2 of the set is the PLRU set:
left (0) at the root, then right (1), then left (0).

When we access a line, we need to change the PLRU data.
We do this by setting ever bit on the path to the accesss to point away from that access.
For example, if we accessed line number 3 in the previous example we'd now have:

<center>
<svg viewBox="-38 -30 156 73" style="width:15.6em">
<g style="font-size:8pt;text-anchor:middle">
<text x="40" y="-20" font-weight="bold" fill="red">1</text>
<text x="0" y="0" font-weight="bold" fill="red">0</text>
<text x="80" y="0">1</text>
<text x="-20" y="20">0</text>
<text x="20" y="20" font-weight="bold" fill="red">0</text>
<text x="60" y="20">1</text>
<text x="100" y="20">0</text>
<!-- -->
<text x="-30" y="40">0</text>
<text x="-10" y="40">1</text>
<text x="10" y="40">2</text>
<text x="30" y="40">3</text>
<text x="50" y="40">4</text>
<text x="70" y="40">5</text>
<text x="90" y="40">6</text>
<text x="110" y="40">7</text>
</g>
<g style="stroke:black;fill:none">
<line x1="35" x2="5" y1="-18" y2="-10"/>
<line x1="45" x2="75" y1="-18" y2="-10"/>
<line x1="-5" x2="-15" y1="2" y2="10"/>
<line x1="5" x2="15" y1="2" y2="10"/>
<line x1="75" x2="65" y1="2" y2="10"/>
<line x1="85" x2="95" y1="2" y2="10"/>
<line x1="-23" x2="-27" y1="22" y2="30"/>
<line x1="-17" x2="-13" y1="22" y2="30"/>
<line x1="17" x2="13" y1="22" y2="30"/>
<line x1="23" x2="27" y1="22" y2="30"/>
<line x1="57" x2="53" y1="22" y2="30"/>
<line x1="63" x2="67" y1="22" y2="30"/>
<line x1="97" x2="93" y1="22" y2="30"/>
<line x1="103" x2="107" y1="22" y2="30"/>
<rect x="-36" y="30" width="12" height="12"/>
<rect x="-16" y="30" width="12" height="12"/>
<rect x="4" y="30" width="12" height="12"/>
<rect x="24" y="30" width="12" height="12"/>
<rect x="44" y="30" width="12" height="12"/>
<rect x="64" y="30" width="12" height="12"/>
<rect x="84" y="30" width="12" height="12"/>
<rect x="104" y="30" width="12" height="12"/>
</g>
</svg>
</center>

with the new PLRU being line number 6.

:::exercise
Suppose the bits of an 8-way Tree-PLRU are all 0s,
meaning the PLRU line is number 0,
and we access line number 0.
What is the shortest possible access sequence that will again make line number 0 the PLRU line?

The answer is in a footnote.[^minswap]
:::

[^minswap]:
    After accessing line 0, three bits are set to 1 that all need setting back to 0 before line 0 will again be the PLRU line.
    We can flip the last of these by accessing line 1,
    the next by accessing line 2 or line 3,
    and the last by accessing any of lines 4 through 7.
    
    Thus, one example access sequence that will work is 0 (given),
    1, 2, 4.

:::aside
Each leaf in this tree can be categorized by the number of bits you'd need to flip in order to make it the PLRU element.
In our fist example tree, that is

<center>
<svg viewBox="-38 -30 156 73" style="width:15.6em">
<g style="font-size:8pt;text-anchor:middle">
<text x="40" y="-20">0</text>
<text x="0" y="0">1</text>
<text x="80" y="0">1</text>
<text x="-20" y="20">0</text>
<text x="20" y="20">0</text>
<text x="60" y="20">1</text>
<text x="100" y="20">0</text>
<g fill="red">
<text x="-30" y="40">1</text>
<text x="-10" y="40">2</text>
<text x="10" y="40">0</text>
<text x="30" y="40">1</text>
<text x="50" y="40">3</text>
<text x="70" y="40">2</text>
<text x="90" y="40">1</text>
<text x="110" y="40">2</text>
</g>
</g>
<g style="stroke:black;fill:none">
<line x1="35" x2="5" y1="-18" y2="-10"/>
<line x1="45" x2="75" y1="-18" y2="-10"/>
<line x1="-5" x2="-15" y1="2" y2="10"/>
<line x1="5" x2="15" y1="2" y2="10"/>
<line x1="75" x2="65" y1="2" y2="10"/>
<line x1="85" x2="95" y1="2" y2="10"/>
<line x1="-23" x2="-27" y1="22" y2="30"/>
<line x1="-17" x2="-13" y1="22" y2="30"/>
<line x1="17" x2="13" y1="22" y2="30"/>
<line x1="23" x2="27" y1="22" y2="30"/>
<line x1="57" x2="53" y1="22" y2="30"/>
<line x1="63" x2="67" y1="22" y2="30"/>
<line x1="97" x2="93" y1="22" y2="30"/>
<line x1="103" x2="107" y1="22" y2="30"/>
<rect x="-36" y="30" width="12" height="12"/>
<rect x="-16" y="30" width="12" height="12"/>
<rect x="4" y="30" width="12" height="12"/>
<rect x="24" y="30" width="12" height="12"/>
<rect x="44" y="30" width="12" height="12"/>
<rect x="64" y="30" width="12" height="12"/>
<rect x="84" y="30" width="12" height="12"/>
<rect x="104" y="30" width="12" height="12"/>
</g>
</svg>
</center>

In a sense, the "1 change needed" leaves are all lumped into one equivalence class of "next-to-least recently used"
and any of them can become the next PLRU with a single access.
:::

# State bits

For a $w$-way set, where $w$ is a power of 2,
we need $w-1$ bits of state (the interior nodes in the tree).
With those bits, we need to efficiently perform two operations:

1. Given the state bits, identify which line to replace (i.e., the PLRU line).
2. Given a line access, update the state bits to make that line the most recently used.

## Encoding the bits

We'll store the bits of state as a binary number with the root bit as the low-order bit, it's left child next, and so on.

:::example
Ignoring the leaf nodes,
<svg viewBox="-38 -30 156 51" style="width:15.6em">
<g style="font-size:8pt;text-anchor:middle">
<text x="40" y="-20">0</text>
<text x="0" y="0">1</text>
<text x="80" y="0">1</text>
<text x="-20" y="20">0</text>
<text x="20" y="20">0</text>
<text x="60" y="20">1</text>
<text x="100" y="20">0</text>
<!-- -->
<text x="-30" y="40">0</text>
<text x="-10" y="40">1</text>
<text x="10" y="40">2</text>
<text x="30" y="40">3</text>
<text x="50" y="40">4</text>
<text x="70" y="40">5</text>
<text x="90" y="40">6</text>
<text x="110" y="40">7</text>
</g>
<g style="stroke:black;fill:none">
<line x1="35" x2="5" y1="-18" y2="-10"/>
<line x1="45" x2="75" y1="-18" y2="-10"/>
<line x1="-5" x2="-15" y1="2" y2="10"/>
<line x1="5" x2="15" y1="2" y2="10"/>
<line x1="75" x2="65" y1="2" y2="10"/>
<line x1="85" x2="95" y1="2" y2="10"/>
<line x1="-23" x2="-27" y1="22" y2="30"/>
<line x1="-17" x2="-13" y1="22" y2="30"/>
<line x1="17" x2="13" y1="22" y2="30"/>
<line x1="23" x2="27" y1="22" y2="30"/>
<line x1="57" x2="53" y1="22" y2="30"/>
<line x1="63" x2="67" y1="22" y2="30"/>
<line x1="97" x2="93" y1="22" y2="30"/>
<line x1="103" x2="107" y1="22" y2="30"/>
<rect x="-36" y="30" width="12" height="12"/>
<rect x="-16" y="30" width="12" height="12"/>
<rect x="4" y="30" width="12" height="12"/>
<rect x="24" y="30" width="12" height="12"/>
<rect x="44" y="30" width="12" height="12"/>
<rect x="64" y="30" width="12" height="12"/>
<rect x="84" y="30" width="12" height="12"/>
<rect x="104" y="30" width="12" height="12"/>
</g>
</svg>
=
0b0100110 = 0x26
:::

There are obviously other orders in which we could store these bits,
but this order will make handling larger ways simpler.

## Identifying the PLRU

The PLRU line number depends on just one path through the state bits.

We can build the line number for any way by starting with 0 (correct for a 1-way set, i.e. direct-mapped)
and traversing down the tree:
at each level we double the line number and add the bit we find.

:::example
When navigating
<svg viewBox="-38 -30 156 73" style="width:15.6em">
<g style="font-size:8pt;text-anchor:middle">
<text x="40" y="-20">1</text>
<text x="0" y="0">1</text>
<text x="80" y="0">1</text>
<text x="-20" y="20">0</text>
<text x="20" y="20">0</text>
<text x="60" y="20">1</text>
<text x="100" y="20">0</text>
<!-- -->
<text x="-30" y="40">0</text>
<text x="-10" y="40">1</text>
<text x="10" y="40">2</text>
<text x="30" y="40">3</text>
<text x="50" y="40">4</text>
<text x="70" y="40">5</text>
<text x="90" y="40">6</text>
<text x="110" y="40">7</text>
</g>
<g style="stroke:black;fill:none">
<line x1="35" x2="5" y1="-18" y2="-10"/>
<line x1="45" x2="75" y1="-18" y2="-10"/>
<line x1="-5" x2="-15" y1="2" y2="10"/>
<line x1="5" x2="15" y1="2" y2="10"/>
<line x1="75" x2="65" y1="2" y2="10"/>
<line x1="85" x2="95" y1="2" y2="10"/>
<line x1="-23" x2="-27" y1="22" y2="30"/>
<line x1="-17" x2="-13" y1="22" y2="30"/>
<line x1="17" x2="13" y1="22" y2="30"/>
<line x1="23" x2="27" y1="22" y2="30"/>
<line x1="57" x2="53" y1="22" y2="30"/>
<line x1="63" x2="67" y1="22" y2="30"/>
<line x1="97" x2="93" y1="22" y2="30"/>
<line x1="103" x2="107" y1="22" y2="30"/>
<rect x="-36" y="30" width="12" height="12"/>
<rect x="-16" y="30" width="12" height="12"/>
<rect x="4" y="30" width="12" height="12"/>
<rect x="24" y="30" width="12" height="12"/>
<rect x="44" y="30" width="12" height="12"/>
<rect x="64" y="30" width="12" height="12"/>
<rect x="84" y="30" width="12" height="12"/>
<rect x="104" y="30" width="12" height="12"/>
</g>
</svg>
we start at 0;
0×2+1 = 1;
1×2+1 = 3;
3×2+0 = 6 which is the PLRU line number.
:::

Navigating the tree is similar, with an extra +1:
we start at bit 0 (i.e. the 2^0^s place, the low-order bit)
and the bit index after $i$ wen reading bit $b$
is $2i+1+b$.

:::example
Consider
<svg viewBox="-38 -30 156 51" style="width:15.6em">
<g style="font-size:8pt;text-anchor:middle">
<text x="40" y="-20">1</text>
<text x="0" y="0">1</text>
<text x="80" y="0">0</text>
<text x="-20" y="20">0</text>
<text x="20" y="20">0</text>
<text x="60" y="20">1</text>
<text x="100" y="20">0</text>
<!-- -->
<text x="-30" y="40">0</text>
<text x="-10" y="40">1</text>
<text x="10" y="40">2</text>
<text x="30" y="40">3</text>
<text x="50" y="40">4</text>
<text x="70" y="40">5</text>
<text x="90" y="40">6</text>
<text x="110" y="40">7</text>
</g>
<g style="stroke:black;fill:none">
<line x1="35" x2="5" y1="-18" y2="-10"/>
<line x1="45" x2="75" y1="-18" y2="-10"/>
<line x1="-5" x2="-15" y1="2" y2="10"/>
<line x1="5" x2="15" y1="2" y2="10"/>
<line x1="75" x2="65" y1="2" y2="10"/>
<line x1="85" x2="95" y1="2" y2="10"/>
<line x1="-23" x2="-27" y1="22" y2="30"/>
<line x1="-17" x2="-13" y1="22" y2="30"/>
<line x1="17" x2="13" y1="22" y2="30"/>
<line x1="23" x2="27" y1="22" y2="30"/>
<line x1="57" x2="53" y1="22" y2="30"/>
<line x1="63" x2="67" y1="22" y2="30"/>
<line x1="97" x2="93" y1="22" y2="30"/>
<line x1="103" x2="107" y1="22" y2="30"/>
<rect x="-36" y="30" width="12" height="12"/>
<rect x="-16" y="30" width="12" height="12"/>
<rect x="4" y="30" width="12" height="12"/>
<rect x="24" y="30" width="12" height="12"/>
<rect x="44" y="30" width="12" height="12"/>
<rect x="64" y="30" width="12" height="12"/>
<rect x="84" y="30" width="12" height="12"/>
<rect x="104" y="30" width="12" height="12"/>
</g>
</svg>
=
0b0100011.

We start at bit 0, where we find a 1 (0b010011**1**);
then bit 2×0+1+1 = 2, where we find a 0 (0b0100**0**11);
then bit 2×2+1+0 = 5, where we find a 1 (0b0**1**00011).
:::

## Making a line the most-recently used

The bits of a line number tell us how we'd have to navigate the tree to find that line:
the high-order bit is the required root tree bit, etc.
To mark the line as most-recently used
we navigate the tree according to those bits
setting each tree node's bit to the opposite of the corresponding bit of the line number.

:::example
Suppose we want to make line 5 the least-recently used in the following state:
<svg viewBox="-38 -30 156 73" style="width:15.6em">
<g style="font-size:8pt;text-anchor:middle">
<text x="40" y="-20">1</text>
<text x="0" y="0">1</text>
<text x="80" y="0">1</text>
<text x="-20" y="20">0</text>
<text x="20" y="20">0</text>
<text x="60" y="20">1</text>
<text x="100" y="20">0</text>
<!-- -->
<text x="-30" y="40">0</text>
<text x="-10" y="40">1</text>
<text x="10" y="40">2</text>
<text x="30" y="40">3</text>
<text x="50" y="40">4</text>
<text x="70" y="40">5</text>
<text x="90" y="40">6</text>
<text x="110" y="40">7</text>
</g>
<g style="stroke:black;fill:none">
<line x1="35" x2="5" y1="-18" y2="-10"/>
<line x1="45" x2="75" y1="-18" y2="-10"/>
<line x1="-5" x2="-15" y1="2" y2="10"/>
<line x1="5" x2="15" y1="2" y2="10"/>
<line x1="75" x2="65" y1="2" y2="10"/>
<line x1="85" x2="95" y1="2" y2="10"/>
<line x1="-23" x2="-27" y1="22" y2="30"/>
<line x1="-17" x2="-13" y1="22" y2="30"/>
<line x1="17" x2="13" y1="22" y2="30"/>
<line x1="23" x2="27" y1="22" y2="30"/>
<line x1="57" x2="53" y1="22" y2="30"/>
<line x1="63" x2="67" y1="22" y2="30"/>
<line x1="97" x2="93" y1="22" y2="30"/>
<line x1="103" x2="107" y1="22" y2="30"/>
<rect x="-36" y="30" width="12" height="12"/>
<rect x="-16" y="30" width="12" height="12"/>
<rect x="4" y="30" width="12" height="12"/>
<rect x="24" y="30" width="12" height="12"/>
<rect x="44" y="30" width="12" height="12"/>
<rect x="64" y="30" width="12" height="12"/>
<rect x="84" y="30" width="12" height="12"/>
<rect x="104" y="30" width="12" height="12"/>
</g>
</svg>

5 = 0b101 and we work high- to low-order bit

0b**1**01, so we set the root to 0 and go right
<svg viewBox="-38 -30 156 73" style="width:15.6em">
<g style="font-size:8pt;text-anchor:middle">
<text x="40" y="-20" fill="red" font-weight="bold">0</text>
<text x="0" y="0">1</text>
<text x="80" y="0">1</text>
<text x="-20" y="20">0</text>
<text x="20" y="20">0</text>
<text x="60" y="20">1</text>
<text x="100" y="20">0</text>
<!-- -->
<text x="-30" y="40">0</text>
<text x="-10" y="40">1</text>
<text x="10" y="40">2</text>
<text x="30" y="40">3</text>
<text x="50" y="40">4</text>
<text x="70" y="40">5</text>
<text x="90" y="40">6</text>
<text x="110" y="40">7</text>
</g>
<g style="stroke:black;fill:none">
<line x1="35" x2="5" y1="-18" y2="-10"/>
<line x1="45" x2="75" y1="-18" y2="-10" stroke="red"/>
<line x1="-5" x2="-15" y1="2" y2="10"/>
<line x1="5" x2="15" y1="2" y2="10"/>
<line x1="75" x2="65" y1="2" y2="10"/>
<line x1="85" x2="95" y1="2" y2="10"/>
<line x1="-23" x2="-27" y1="22" y2="30"/>
<line x1="-17" x2="-13" y1="22" y2="30"/>
<line x1="17" x2="13" y1="22" y2="30"/>
<line x1="23" x2="27" y1="22" y2="30"/>
<line x1="57" x2="53" y1="22" y2="30"/>
<line x1="63" x2="67" y1="22" y2="30"/>
<line x1="97" x2="93" y1="22" y2="30"/>
<line x1="103" x2="107" y1="22" y2="30"/>
<rect x="-36" y="30" width="12" height="12"/>
<rect x="-16" y="30" width="12" height="12"/>
<rect x="4" y="30" width="12" height="12"/>
<rect x="24" y="30" width="12" height="12"/>
<rect x="44" y="30" width="12" height="12"/>
<rect x="64" y="30" width="12" height="12"/>
<rect x="84" y="30" width="12" height="12"/>
<rect x="104" y="30" width="12" height="12"/>
</g>
</svg>


0b1**0**1, so we set the node to 1 and go left
<svg viewBox="-38 -30 156 73" style="width:15.6em">
<g style="font-size:8pt;text-anchor:middle">
<text x="40" y="-20" fill="red">0</text>
<text x="0" y="0">1</text>
<text x="80" y="0" fill="red" font-weight="bold">1</text>
<text x="-20" y="20">0</text>
<text x="20" y="20">0</text>
<text x="60" y="20">1</text>
<text x="100" y="20">0</text>
<!-- -->
<text x="-30" y="40">0</text>
<text x="-10" y="40">1</text>
<text x="10" y="40">2</text>
<text x="30" y="40">3</text>
<text x="50" y="40">4</text>
<text x="70" y="40">5</text>
<text x="90" y="40">6</text>
<text x="110" y="40">7</text>
</g>
<g style="stroke:black;fill:none">
<line x1="35" x2="5" y1="-18" y2="-10"/>
<line x1="45" x2="75" y1="-18" y2="-10" stroke="red"/>
<line x1="-5" x2="-15" y1="2" y2="10"/>
<line x1="5" x2="15" y1="2" y2="10"/>
<line x1="75" x2="65" y1="2" y2="10" stroke="red"/>
<line x1="85" x2="95" y1="2" y2="10"/>
<line x1="-23" x2="-27" y1="22" y2="30"/>
<line x1="-17" x2="-13" y1="22" y2="30"/>
<line x1="17" x2="13" y1="22" y2="30"/>
<line x1="23" x2="27" y1="22" y2="30"/>
<line x1="57" x2="53" y1="22" y2="30"/>
<line x1="63" x2="67" y1="22" y2="30"/>
<line x1="97" x2="93" y1="22" y2="30"/>
<line x1="103" x2="107" y1="22" y2="30"/>
<rect x="-36" y="30" width="12" height="12"/>
<rect x="-16" y="30" width="12" height="12"/>
<rect x="4" y="30" width="12" height="12"/>
<rect x="24" y="30" width="12" height="12"/>
<rect x="44" y="30" width="12" height="12"/>
<rect x="64" y="30" width="12" height="12"/>
<rect x="84" y="30" width="12" height="12"/>
<rect x="104" y="30" width="12" height="12"/>
</g>
</svg>

0b10**1**, so we set the node to 0 and go right
<svg viewBox="-38 -30 156 73" style="width:15.6em">
<g style="font-size:8pt;text-anchor:middle">
<text x="40" y="-20" fill="red">0</text>
<text x="0" y="0">1</text>
<text x="80" y="0" fill="red">1</text>
<text x="-20" y="20">0</text>
<text x="20" y="20">0</text>
<text x="60" y="20" fill="red" font-weight="bold">0</text>
<text x="100" y="20">0</text>
<!-- -->
<text x="-30" y="40">0</text>
<text x="-10" y="40">1</text>
<text x="10" y="40">2</text>
<text x="30" y="40">3</text>
<text x="50" y="40">4</text>
<text x="70" y="40">5</text>
<text x="90" y="40">6</text>
<text x="110" y="40">7</text>
</g>
<g style="stroke:black;fill:none">
<line x1="35" x2="5" y1="-18" y2="-10"/>
<line x1="45" x2="75" y1="-18" y2="-10" stroke="red"/>
<line x1="-5" x2="-15" y1="2" y2="10"/>
<line x1="5" x2="15" y1="2" y2="10"/>
<line x1="75" x2="65" y1="2" y2="10" stroke="red"/>
<line x1="85" x2="95" y1="2" y2="10"/>
<line x1="-23" x2="-27" y1="22" y2="30"/>
<line x1="-17" x2="-13" y1="22" y2="30"/>
<line x1="17" x2="13" y1="22" y2="30"/>
<line x1="23" x2="27" y1="22" y2="30"/>
<line x1="57" x2="53" y1="22" y2="30"/>
<line x1="63" x2="67" y1="22" y2="30" stroke="red"/>
<line x1="97" x2="93" y1="22" y2="30"/>
<line x1="103" x2="107" y1="22" y2="30"/>
<rect x="-36" y="30" width="12" height="12"/>
<rect x="-16" y="30" width="12" height="12"/>
<rect x="4" y="30" width="12" height="12"/>
<rect x="24" y="30" width="12" height="12"/>
<rect x="44" y="30" width="12" height="12"/>
<rect x="64" y="30" width="12" height="12"/>
<rect x="84" y="30" width="12" height="12"/>
<rect x="104" y="30" width="12" height="12"/>
</g>
</svg>
:::

