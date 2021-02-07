#include <catch.hpp>

#include <sfe/sfe.hpp>

TEST_CASE("Basic") {
  REQUIRE(!sfe::get_current_exception_stacktrace());
  try {
    REQUIRE(!sfe::get_current_exception_stacktrace());
    throw std::runtime_error("error");
  } catch (const std::exception& exc) {
    REQUIRE(!sfe::get_current_exception_stacktrace());
  }
  REQUIRE(!sfe::get_current_exception_stacktrace());
  try {
    REQUIRE(!sfe::get_current_exception_stacktrace());
    throw 42;
  } catch (int exc) {
    REQUIRE(!sfe::get_current_exception_stacktrace());
  }
  REQUIRE(!sfe::get_current_exception_stacktrace());
}
