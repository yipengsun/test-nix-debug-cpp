# test-nix-debug-cpp

Test debugging C++ programs within a nix devShell


## Usage

First go inside a nix devShell, then:

```shell
cmake --preset debug && cmake --build build/Debug
cgdb build/Debug/main

# inside cgdb
b main  # add a break point at start of main()
b 6     # add a break point at line 6 of current file

run

n    # step-by-step debug
c    # run til next break point
p n  # print local variable `n`
s    # step into function

info break
del 2  # delete #2 break point
```
