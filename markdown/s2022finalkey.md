---
title: Key to spring 2022 final
...

1. `(x>>8)|(x<<8)`
2. `while(f(s)) { t += 1; s += 2; }`
3. 
        r  3330  3  3   [ ]  [ ]  [ ]   set3: [3, ]             cold miss
        w  3340  3  3   [*]  [ ]  [ ]   set3: [3*, ]            hit
        r  3430  4  3   [ ]  [ ]  [ ]   set4: [4, ]             cold miss
        w  4330  3  4   [ ]  [ ]  [ ]   set3: [4*, 3*]          cold miss
        r  5330  3  5   [ ]  [*]  [*]   set3: [5, 4*] evict 3*  cold miss
        w  3530  5  3   [ ]  [ ]  [ ]   set5: [3*, ]            cold miss
        r  3330  3  3   [ ]  [*]  [*]   set3: [3, 5] evict 4*   conflict
4.
    a. 5%
    b. 8.0

5. `for(int i=0; i<37; i+=1) use(array[i]);`

6. 
    a. all four the same
    b. `foo1`
    c. `foo3` and `foo4`

7. 16
8. 12
9. 
    a. 9
    b. 16
    c. all three
    d. all but the first

10.
    a. E
    b. D
    c. A

11. C
12. B and C
13. 
    a. NBBN
    b. typo; accept both SSSBN and SSBNN
14.
    a. 3
    b. A or C
15.
    a. incorrectly
    b. memory
16.
    a. first two; 5th is ambiguous and was dropped
    b. 3
17.
    a. A and D
    b. B
