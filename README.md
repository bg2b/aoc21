# Advent of Code 2021

Info and problems are available at https://adventofcode.com/2021

Each day's problem input and solution is in a subdirectory, `01`, `02`, etc.

Solutions are mostly C++: problem 1 is `doit1.cc`, problem 2 is `doit2.cc`.
Compile with
```
g++ -Wall -g -o doit1 doit1.cc
```
Input is on stdin, output is printed to stdout.

Input has sometimes been pre-processed for ease of reading.
Example input if any is called `input1`.
The real input is `input`.

Sometimes I might go back and revisit a problem in a different
(usually more efficient) way.  `doit2a.cc` would be an alternative to
`doit2.cc`.

Day 4 uses a shell script, `play1.sh` and `play2.sh` for the two
parts.

Occasionally there's only one program used for both parts.
