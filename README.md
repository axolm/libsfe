# libsfe
Library for extracting stacktrace from exception.

### Install (Tested only for Linux)
```sh
$ git clone https://github.com/axolm/libsfe.git
$ cd libsfe
$ sudo make install
```

### Motivational example
[basic.cpp](examples/basic.cpp) file
```cpp
#include <iostream>
#include <sfe/sfe.hpp>
#include <stdexcept>

void throw_something() {
  throw std::runtime_error("SOME ERROR");
}

int main() {
  try {
    throw_something();
  } catch (const std::exception& exc) {
    auto trace = sfe::get_current_exception_stacktrace();
    std::cerr << "Caught exception. Err: `" << exc.what() << "`, Trace:\n"
              << trace;
  }
  return 0;
}
```
Compiling and runing
```
$ g++ -g -DBOOST_STACKTRACE_DYN_LINK basic.cpp -obasic \
    -lsfe_basic -lboost_stacktrace_backtrace -ldl

$ ./basic
Caught exception. Err: `SOME ERROR`, Trace:

# Debug info without recompiling!
$ LD_PRELOAD=/usr/local/lib/libsfe_preload.so ./basic
Caught exception. Err: `SOME ERROR`, Trace:
 0# throw_something() at /home/axolm/basic.cpp:6
 1# main at /home/axolm/basic.cpp:17
 2# 0x00007FB823D77BF7 in /lib/x86_64-linux-gnu/libc.so.6
 3# 0x0000558E89A2202A in ./basic
```

### How does it work
* `libsfe_basic.so` just returns empty stacktrace from `sfe::get_current_exception_stacktrace()`.
* `libsfe_preload.so` overrides function `__cxa_allocate_exception`, dumps `boost::stacktrace` near exception obj and in `sfe::get_current_exception_stacktrace()` loads it and returns.

### Deps
* libdl
* [boost/stacktrace](https://github.com/boostorg/stacktrace)
