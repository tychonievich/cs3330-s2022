---
title: Components of Our Processors
...

We will build several processors in this class, using the following abstractions of hardware components.

# Need to Know: Components and what they do

## Wire (groups)

Physical wires are conductive paths surrounded by insulators.
They connect components on out chip.

Because of the way the chip components work, each wire shall be connected to **exactly one** signal producer.
Each wire may fork and connect to as many signal consumers as we wish.

Trying to connect a single wire to multiple signal producers would create a short circuit, which is not just wasteful of energy and incorrect in operation, but also likely to generate enough heat to destroy the circuit.

We will often want multi-bit values. Each physical wire can only handle one bit at a time, but we can run groups of wires in parallel to create multi-bit values. In this class we'll often call such a wire group a "multi-bit wire" as a kind of shorthand to simplify conversation

In HCLRS, we have special notation for wire groups:

```hclrs
wire mywirename:4;   # declares a 4-bit-wide wire group
```

## Parts of wire groups

It requires no special logic to peel off some wires out of a wire group: we simply have to route those wires differently than the others.
It also requires not special logic to take several small wire groups and run them together as a larger wire group.

In HCLRS, we have special notation to represent this splitting and concatenating

```c
wire2 = wire1[1..4];    # selects bits 1, 2, and 3 from wire1
wire3 = wire1 .. wire2; # concatenates two wire groups
```

## Register (banks)

A register is a special circuit that conceptually has the following parts:

- Storage, which has either a 0 or 1 in it any given time
- Output, which is always the same as storage
- Input, which is ignored except when the clock does certain things
- Clock input, a special input attached to an oscillator (a device that alternates 1 and 0 at predictable intervals)

The storage only changes at the moment when the clock transitions from 0 to 1 (called a "rising clock edge").
At that moment, the storage (and thus the output) changes to match the input.
The time from one rising clock edge to the next is called a "cycle".

We will often want to store multi-bit values. We do this by lining up one single-bit register for each bit we want to store. Such assemblages are called "multi-bit registers," and are so dominant that often they are just called "registers" with the simpler single-bit parts they are made of called "single-bit registers" if we need to refer to them.

We will often want to group a set of registers together conceptually; we will call such a collection a "register bank".

We will sometimes want to bypass usual register behavior in one of two ways:

- Suspend (also called stall): ignore the clock for one cycle, keeping the current state unchanged
- Reset (also called bubble): ignore the input for one cycle and revert to a default value

Typically we'd do these bypasses on all the registers in a register back together.

In HLCRS, we have special notation for register banks that use two-letter naming to match the textbook's notation:

```
register fE { hi:3 = 2; there:5 = 0; }
f_hi = 5;   # the input value hi will assume next cycle
x = E_hi;   # reads the value hi is currently outputting
bubble_E = false;  # if true, hi becomes 2 and there 0 next cycle
stall_E = false;   # if true, hi and there unchanged next cycle
```

## Register File

Most registers have to be directly wired up to their inputs, which is a limitation we don't want to have for program registers -- that is, the registers that can be used as operands in our ISA.
Instead, we'll place those registers in a special construct called a "register file".

A register file conceptually works like an array of registers.
We can read from any index of that array and write to any index.
Under the hood a register file that is created using registers and muxes, but that's an implementation detail.

In our textbook and in HCLRS, we have pre-built register bank
with fifteen 64-bit registers numbered 0 though 14
and have it wired up with two read ports
and two write ports that collectively work like the code

```c
reg_outputA = register[reg_srcA];
reg_outputB = register[reg_srcB];
register[reg_dstE] = ref_inputE;
register[reg_dstM] = ref_inputM;
```

## Memory

While discussing simple processors, we'll use a simplified model of memory:
it's a huge array of bytes and we read and write values by address.

In our textbook and in HCLRS, we have pre-built memory interface
with on 10-byte read port for fetching instructions
and an 8-byte read/write port for interacting with data

```c
i10bytes = memory[pc];
if (mem_writebit) memory[mem_addr] = mem_input;
else if (mem_readbit) mem_output = memory[mem_addr];
```

When we talk about fancier processors we'll consider a more nuanced memory model
where operations can take variable numbers of cycles.
A simple way that could be implemented from the processor perspective is with the memory system providing a "ready" signal that is 0 if the data is still being fetched and 1 if it is ready.

## Muxes

A Mux (short for **mu**ltiple**x**er) is simple but important circuit.
It has two kinds of inputs:
2^*k*^ different data inputs, all the same bit width,
and a *k*-bit selector input.
It has one output, which is the same as one of its data inputs;
which one depends on the value of the selector input.

A mux can be likened to a conditional expression,
and indeed it is written as one in some hardware description languages:

```c
mux_result =
    selector == 0 ? input_0 :
    selector == 1 ? input_1 :
    selector == 2 ? input_2 :
    selector == 3 ? input_3 ;
```

It can also be likened to a case statement

```c
switch(selector) {
    case 0: mux_result = input_0; break;
    case 1: mux_result = input_1; break;
    case 2: mux_result = input_2; break;
    case 3: mux_result = input_3; break;
}
```

or to an array index

```c
mux_result = input_values[selector];
```


Muxes will appear *everywhere* in our processor design.
HCLRS has a special syntax just for them, one which is also able to use non-power-of-two input counts and to do fancier comparisons in picking an input.

```
some_wire = [
    x == 1 : 3; 
    x == 2 && y < 3 : 4;
    x == 3 : 5;
    1 : 0;
]
```

## Everything else

Anything you can write using C/C++ variables and operators
without pointer operations
can be implemented in hardware using some assemblage of transistors,
provided the wire widths make sense.

Rather than define new syntax for this, both the textbook and HCLRS let you just write expressions with wire names as variables.

# Nice to know

If you took ECE 2330, everything in this section is (theoretically) review.
If not, it is likely all new.
ECE 2330 is not a prereq for this course, so nothing in this section will be tested in quizzes or exams.


## 0 and 1

Claude Shannon founded the field of information theory.
A core fact in information theory is that there is a basic unit of information,
called a "bit^[a portmanteau of "binary digit"]" or a "bit of entropy."
Roughly speaking, a "bit" is an amount of information that is about as surprising as the result of a single coin flip.
In the sentence "please pass the salt" the word "salt" has less than a bit of entropy; most of the time someone says "please pass the" the next word they say is "salt," so adding that word provides very little more information.
On the other hand, the word that comes after "is" in "Hello, my name is" has many bits of entropy; no matter what the word is, it was quite surprising.

:::exercise
Claude Shannon performed an experiment to determine the bits of entropy
of the average letter in English.
If you wish, you can recreate that experiment as follows:

1. Find a large corpus of example text (the larger the better)
2. Write a program that repeatedly
    1. selects a random substring from the corpus, long enough to give a little context (perhaps 50 characters)
    2. prompt the user to type what the next character will be
    3. track the number of correct and incorrect guesses

After guessing correctly on *r* out of *g* total tries,
an estimate of the bits of entropy per character in the corpus is
log~2~(*g* ÷ *r*).

Not all languages have the same information per character.
Shannon suggested English had roughly 1 bit per letter^[Shannon, Cluade E. (1950), "Prediction and Entropy of Printed English", *Bell Systems Technical Journal* (3) pp. 50--64.],
while languages that use ideograms have many more.

It is likely that bits per second of speaking is more constant across languages,
which could probably be tested by measuring the bits per character of subtitles
and combing it with the timing information the subtitles contain,
but I am unaware of any published effort to do this or any related measurement.
:::

How many bits of information can we transmit over a wire?
If we put voltage on one end, because wire conducts well we'll very soon see the same voltage at the other end.
Presumably, the more precise our measurement of that voltage can be, the more information we can collect.
If we can distinguish between 1000 voltage levels, for example, we'll get log~2~(1000) = 10 bits of information per measurement; whereas a less sensitive voltmeter that can only distinguish between 2 voltage levels gets only log~2~(2) = 1 bit per reading.
This seems to suggest that more precise voltage generators and sensors allow more information to be transmitted per reading.

The problem with this assumption is that it takes longer to transmit higher-resolution data.
This is partly a consequence of fundamental mathematical and physical laws, like the Heisenberg uncertainty principle,
but it is also more tellingly a consequence of living in a noisy world.
To tell the difference between 8.35 and 8.34 volts, you need to ensure that the impact of wire quality and the environment through which it passes contributes significantly less than 0.01 voltage error to the measurement; generally this requires watching the line for long enough to see what part of the voltage is noisy variations and what part is the true signal.
By contrast, telling the difference between 10 volts and 0 volts is much simpler and much more robust against noise.
It is quite possible to make several good 1-bit measurements in the time it'd take to make one 10-bit measurement.

This observation led to advances in digital signals:
signals composed of a large number of simple "digits^["Digit" comes from a Latin word meaning "finger" and, prior to the computer age, was used to refer to the ten basic numerals 0 through 9. "Digital" meaning "a signal categorized into one of a small number of distinct states" became common in the 1960s, though it was used by computing pioneers as early as the late 1930s.]" instead of one fine-grained "analog^["Analog" (or "Analogue" outside the USA) comes from a word meaning "similar to" or "along side", suggesting that an analog signal has some direct correlation to the thing it represents.]" signal.
We can communicate more information with less error and less expensive tools
by transmitting a large number of single-bit signals
instead of transmitting a smaller number of signals with more information in each.

A 1-bit signal is measured as being in one of two states.
We often call those states "0" and "1",
though sometimes we use other names:
"false" and "true",
"low voltage" and "high voltage",
"off" and "on",
etc.

## Current vs Voltage

Ignoring many of the nuances of electricity and speaking only in broad generalities,

Current
:   is the rate at which electrons flow.
    In practice it is not always electrons that are the conceptual charge carriers
    and amperage does not map directly to electrons/second, but that is the general idea.

    If electrical state changes, current is involved.

Voltage
:   is the desire electrons have to flow; the electrical pressure, if you will.
    Voltage does not always cause current because it contends against resistance: not all substances conduct electricity very easily.

    Most elements of computer hardware are sensitive to voltage.
    The "1"s are "high voltage" and "0"s are "low voltage".
    How high depends on the underlying design of the components,
    but chips with highs of around 1 volt are common today (2018).
    Low voltage is generally close to 0 volts.

## Transistors and Gates

There are several ways of building a transistor, but all of them are versions of an electrical switch.
There's two wires that might be connected or might be separated;
which one they are is controlled by changing the voltage applied to a third wire.

We use transistors to build larger, more useful components.
The next step up from a transistor is a gate.
Gates are made by attaching a few transistors in a special arrangement to both a source of 1 and 0:
an electrical source and an electrical sink.

<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 52.916673 63.500004">
<g transform="translate(-5.2916703,-26.458334)">
  <path d="m 37.041667,58.208334 v 5.291667 h -5.291666 v 10.583333 h 5.291666 V 79.375" style="fill:none;stroke:#000000;stroke-width:0.264583px" />
  <path d="M 29.104166,63.499999 V 74.083332" style="fill:none;stroke:#000000;stroke-width:0.264583px" />
  <path d="m 21.166667,68.791667 7.937499,-10e-7" style="fill:none;stroke:#000000;stroke-width:0.264583px" />
</g>
<g transform="translate(-5.2916633,-47.625002)">
  <path d="m 37.041667,58.208334 v 5.291667 h -5.291666 v 10.583333 h 5.291666 V 79.375" style="fill:none;stroke:#000000;stroke-width:0.264583px" />
  <path d="M 29.104166,63.499999 V 74.083332" style="fill:none;stroke:#000000;stroke-width:0.264583px" />
  <path d="m 21.166667,68.791667 7.937499,-10e-7" style="fill:none;stroke:#000000;stroke-width:0.264583px" />
  <circle r="1.3229166" cy="68.791664" cx="27.781248" style="fill:#ffffff;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.264583;stroke-opacity:1" />
</g>
<path d="m 31.749998,31.749995 7.937492,1e-6" style="fill:none;stroke:#000000;stroke-width:0.264583px" />
<rect y="31.220839" x="15.345847" height="1.0583333" width="1.0583333" style="fill:#ffffff;fill-opacity:1;stroke:none;stroke-width:0.264583;stroke-opacity:1" />
<rect y="30.956259" x="15.081267" height="1.587497" width="1.587497" style="fill:#ffffff;fill-opacity:1;stroke:none;stroke-width:0.264583;stroke-opacity:1" />
<rect y="41.804173" x="-11.112493" height="1.0583333" width="1.0583333" style="fill:#ffffff;fill-opacity:1;stroke:none;stroke-width:0.264583;stroke-opacity:1" />
<rect y="41.539593" x="-11.377088" height="1.5874995" width="1.587502" style="fill:#ffffff;fill-opacity:1;stroke:none;stroke-width:0.264583;stroke-opacity:1" />
<path d="M 15.874998,21.166662 V 31.74999 H 7.9374983" style="fill:none;stroke:#000000;stroke-width:0.264583px" />
<text y="33.337502" x="4.2333293" style="font-style:normal;font-weight:normal;font-size:4.23333px;line-height:1.25;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.264583" xml:space="preserve"><tspan y="33.337502" x="4.2333293" style="font-size:4.23333px;stroke-width:0.264583">A</tspan></text>
<text y="33.337502" x="41.275005" style="font-style:normal;font-weight:normal;font-size:4.23333px;line-height:1.25;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.264583" xml:space="preserve"><tspan y="33.337502" x="41.275005" style="font-size:4.23333px;stroke-width:0.264583">B</tspan></text>
<text y="7.9374967" x="13.229177" style="font-size:4.23333px;line-height:1.25;font-family:sans-serif;stroke-width:0.264583" xml:space="preserve"><tspan y="7.9374967" x="13.229177" style="stroke-width:0.264583">source of voltage</tspan></text>
<text y="57.150032" x="23.812489" style="font-size:4.23333px;line-height:1.25;font-family:sans-serif;stroke-width:0.264583" xml:space="preserve"><tspan y="57.150032" x="23.812489" style="stroke-width:0.264583">ground</tspan></text>
<path d="m 15.875,31.75 c 0,10.583333 0,10.583333 0,10.583333" style="fill:none;stroke:#000000;stroke-width:0.264583px" />
</svg>
