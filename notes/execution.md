NOTE: for any of your changes to be reflected when running souper, you first need to remake the build directory (cmake and make).

# Execution

## souper, souper-check

There are two main ways of executing Souper, souper and souper-check.

1. souper

Execution is detailed in the main souper README. The best way is the first method, (compile c code with clang, then run souper with it).

The actual file for soupers functionality is souper/tools/souper.cpp

Souper (despite its -help option saying so) doesn’t seem to have pruning implemented, so in most cases you’ll be using souper-check

2. souper-check

It takes files as input with the following syntax:
```
(souper instructions for the LHS)
infer (LHS result)
(souper instructions for the RHS)
result (RHS result)
```
Remember, the LHS is the specification and the RHS is the candidate.

Examples of test files meant for souper-check are in souper/test/Infer/pruning, and also in autoprune.
The actual file for souper-check’s functionality is souper/tools/souper-check.cpp

An example of running souper-check to test pruning:
```
build/souper-check -try-dataflow-pruning -souper-debug-level=3 test/Infer/pruning/dataflow1.opt
```

# utils folder, and general testing notes

A third way to run souper/test your code is using the utils folder. Mainly the cache_infer script, which  counts the optimizations performed and not-performed. It has an option (-test-pruning) that was probably used to make sure pruning  didn’t decrease the amount of optimizations performed. I will note that given code that was supposed to be optimized by souper, it still said nothing was optimized, so I’m not sure if this script is functional. It could probably be used just to check generated candidates.

If you want to use it the setup process is in notes/setup_cacheinfer.md.

In an email sent to someone who worked on the original pruning paper asking about testing pruning using this folder, they said:

"You need to fill a redis cache with synthesis problems and then run `cache-infer -test-pruning` to see discrepancies.  However, if you are implementing new pruning techniques it would be easier to write a script comparing the outputs of souper-check with vs without pruning enabled"

There’s a big file of souper-checks in autoprune that you can use, however the origin of the tests and how well they reflect generated candidates is unknown.
