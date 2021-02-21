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
