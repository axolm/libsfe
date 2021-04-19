#include <catch.hpp>

#include <sfe/sfe.hpp>

#include <iostream>
#include <unordered_map>

void broken_function() noexcept {
  std::unordered_map<std::string, int> m;
  std::ignore = m.at("non-existing-key");
}

TEST_CASE("std::terminate") {
  std::set_terminate([] {
    auto trace = sfe::get_current_exception_stacktrace();

#ifdef BOOST_STACKTRACE_USE_BACKTRACE
    REQUIRE([&trace] {
      for (auto& frame : trace) {
        if (frame.name().find("broken_function") != std::string::npos) {
          return true;
        }
      }
      return false;
    }());
#endif

    std::exit(0);
  });
  broken_function();
  std::abort();
}
