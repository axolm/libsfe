#include <bits/stdc++.h>
#include <sfe/sfe.hpp>

void test() {
  try {
    throw 4;
  } catch (int x) {
    assert(x == 4);
    assert(not sfe::get_current_exception_stacktrace().empty());
  }

  try {
    throw std::runtime_error("Hello");
  } catch (const std::exception& e) {
    assert(std::string{"Hello"} == e.what());
    assert(not sfe::get_current_exception_stacktrace().empty());

    std::cerr << sfe::get_current_exception_stacktrace();
  }
}

int main() {
  std::cerr << "Start test\n";
  test();
  std::cerr << "Finish test\n";

  return 0;
}
