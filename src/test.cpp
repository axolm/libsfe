#include <bits/stdc++.h>
#include "get_stacktrace_from_exception.h"
// using namespace std;

void f() {
  // throw 4;
  // throw std::array<int, 99>();
  // throw std::runtime_error("Hello");
}

int main() {
  std::cerr << "start\n";
  try {
    f();
  } catch (int x) {
    // Not work when copy
    // std::cerr << std::get_stacktrace_from_exception(x) << '\n';
  } catch (const std::array<int ,99>& arr) {
    std::cerr << std::get_stacktrace_from_exception(arr) << '\n';
  } catch (const std::exception& e) {
    std::cerr << std::get_stacktrace_from_exception(e) << '\n';
  }
  std::cerr << "finish\n";

  return 0;
}
