---
title: "Computer Architecture"
...


Welcome to the work-in-progress class page for CS 3330 &ndash; Computer Architecture.
This material represents the best current estimate of course flow and content.


# Course Content

There are common expectations that this course does not fulfill:

- You won't learn how transistors work or how to combine them into gates; that's ECE 2630.
- You won't learn how to combine gates into processor components; that's ECE/CS 2330.
- You won't learn how to put processor components into physical chips; that's the ECE 4332.

Instead, you will learn

- How computers are built and how that impacts you as a programmer.
- How to make code run quickly, or at least how to avoid making it run slowly.
- A reasonable amount of C, 
  a moderate amount of x86 assembly, 
  and a little of a hardware description language called HCL.
- How hardware guesses what your code will do before it does it.
- More than you knew there was to know about memory and caching.


# Relationship to other courses

CS 3330

- has minimal overlap with ECE/CS 2330.

- assumes you did well in CS 2150. In particular, it assumes you know

    * either C or C++
    * pointers and addresses
    * how the call stack works
    * what assembly and machine language are
    * binary and hexadecimal numbers, basic exponentiation and logarithms

- has enough overlap with ECE 4435 that you may only receive credit for one of those two courses.

- is prerequisite for CS 4414 (OS), CS 4434 (Dependable Systems), CS 4444 (Parallel Computing), CS/ECE 4457 (Networks), and CS 4630 (Defense Against the Dark Arts).  In each cases, either ECE 3430 or ECE 4435 also meets the CS 3330 prerequisite.

There is another section of CS 3330 being offered this semester. The two cover the same content, but in a different order with different assignments and different grading standards. [Historical offerings](https://vagrades.com/uva/CS3330) by these instructors suggest this section will have a significantly lower average grade than the other section.

# Course Staff

**Luther Tychonievich**
:   Office: Rice 208. If the door is open, you are welcome to come in.
    <br/>Email: tychonievich@virginia.edu. I generally check it once a day on business days only.  Always include 3330 in the subject line.
    <br/>Telephone: 434-243-3789

**TAs**
:   Andrew, Austin, Hongyan, Ishaan, Karan, Kathryn, Lukas, Mike, Nathaniel, Noah, Sidhardh, and Yulun.
    
    Please respect the fact that our TAs are also students in other classes, with assignments and so on of their own,
    and limit your 3330-related contact with them to the times they have scheduled to act as TAs.
    
    Unless they tell you otherwise, use Piazza to contact them
    (and make your post visible to all course staff, not just one or two of them).

# Communication channels

## Piazza
This is the place to get asynchronous help for almost any issue. 
Access [via Collab](https://collab.itc.virginia.edu/) or piazza.com.

## Office hours

See [the help page](oh.html).

## Anonymous Feedback

Anonymous feedback, which is available through [Collab](https://collab.itc.virginia.edu/), is appreciated by the instructor.
As it is truly anonymous, responses to it, if needed, will be given during lecture.

# You will need

## Textbook

<img alt="Picture of csapp cover" src="http://csapp.cs.cmu.edu/3e/images/csapp3e-cover.jpg" style="float:right; height:15em; margin-left:1ex;"/>

A copy of Bryant & O'Halleron's *Computer Systems: a Programmer's Perspective*, third edition. The international third edition is okay. [The changes to chapters 3, 4, and 5](http://csapp.cs.cmu.edu/3e/changes3e.html) make the 2nd edition strongly discouraged. The first edition will not work.

<details><summary>
This course is part of a program with The UVA Bookstore providing course material through Collab for a lesser fee.
Click to see the full details from the bookstore, <strong>including how to opt out of paying for the textbook access on Collab</strong> if you have your own print copy instead.
</summary>

ALL students enrolled in the class will have immediate access to your digital course materials through UVA Collab for the first 2 weeks of class???for free. After February 2, your student account will be charged $29.18.

Students will receive an email from UVA Inclusive Access with detailed information about accessing the materials, costs, and opting out.

This program is optional for students but you must actively opt out by the deadline to not be charged. Additionally, you will lose access to your materials.

Due to the special pricing, no refunds can be processed.

This program aims to offer all students accessibility and affordability. If you have any questions regarding the program, please email us at [UVAInclusiveAccess@virginia.edu](mailto:UVAInclusiveAccess@virginia.edu). 
</details>

## Meetings

Lectures meet each Monday and Wednesday at 3:30pm in Rice 130. They will be recorded and posted on the schedule a few hours after each class. Attendance is not required, but past experience suggests attendance is strongly correlated with success.

Labs meet each Wednesday at either 5:00pm or 6:30pm, depending on your section, in Olsson 018. They are not recorded and should be attended in person. You should always attend your assigned section; exceptions can be made in advance by the TAs in your lab section.

If you are unable to attend lab in person, you should submit work before your appointed lab time; if the lab describes in-person check-off, that can also be done during office hours.

## Time

The intent is to meet policy [PROV-022](http://uvapolicy.virginia.edu/policy/PROV-022),
which provides a <q>minimum standard</q> of 1 credit ??? 3 hours of academic work per week for 15 weeks.
We target a level of work where the median student will spend approximately the following each week:

- 2.5 hours lecture
- 1.25 hours lab
- 2 hours reading and online open-book quizzes
- 4 hours homework in weeks with homework; in other weeks, this time will be expected to be spent reading and otherwise studying.

We encourage you to set aside this time on your weekly calender in advance.

# Grading and Academic Honesty

See [this page](policies.html)

# Accommodations

If you need any kind of special accommodations,
including but not limited to 
[disability, learning needs]((http://studenthealth.virginia.edu/sdac),
[religious observances](https://eocr.virginia.edu/accommodations-religious-observance),
illness, or personal circumstances,
please contact us as soon as you are aware of these needs. We aim to be as accommodating and fair as possible.
