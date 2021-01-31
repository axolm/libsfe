#include <catch.hpp>

#include <sfe/sfe.hpp>

#include <iostream>
#include <stdexcept>
#include <utility>

template <typename Func>
void PassDifferentTypes(Func&& func) {
  func(std::runtime_error("hello"));
  func(std::exception());
  func(42);
  func(true);
  func("abacaba");
  func(sfe::stacktrace{});
}

TEST_CASE("Basic") {
  auto lambda_test = [](auto&& arg) {
    using ExcType = std::decay_t<decltype(arg)>;
    try {
      throw std::forward<ExcType>(arg);
    } catch (ExcType& exc) {
      auto trace = sfe::get_current_exception_stacktrace();
      REQUIRE(trace);
      REQUIRE(trace == sfe::get_current_exception_stacktrace());
    }
    REQUIRE_THROWS_AS(sfe::get_current_exception_stacktrace(),
                      sfe::no_stacktrace_error);
  };

  PassDifferentTypes(lambda_test);
}

TEST_CASE("WithRethrow") {
  auto lambda_test = [](auto&& arg) {
    using ExcType = std::decay_t<decltype(arg)>;
    std::exception_ptr ptr;

    try {
      throw std::forward<ExcType>(arg);
    } catch (ExcType& exc) {
      REQUIRE(sfe::get_current_exception_stacktrace());
      ptr = std::current_exception();
      REQUIRE(sfe::get_current_exception_stacktrace());
    }

    try {
      std::rethrow_exception(ptr);
    } catch (...) {
      REQUIRE(sfe::get_current_exception_stacktrace());
    }
    REQUIRE_THROWS_AS(sfe::get_current_exception_stacktrace(),
                      sfe::no_stacktrace_error);
  };

  PassDifferentTypes(lambda_test);
}

TEST_CASE("DeepThrow") {
  auto lambda_test = [](auto&& arg) {
    using ExcType = std::decay_t<decltype(arg)>;

    sfe::stacktrace trace1, trace2, trace3;

    REQUIRE_THROWS_AS(sfe::get_current_exception_stacktrace(),
                      sfe::no_stacktrace_error);
    try {
      try {
        try {
          throw std::forward<ExcType>(arg);
        } catch (ExcType& exc) {
          trace1 = sfe::get_current_exception_stacktrace();
          throw;
        }
      } catch (ExcType& exc) {
        trace2 = sfe::get_current_exception_stacktrace();
        throw exc;
      }
    } catch (...) {
      trace3 = sfe::get_current_exception_stacktrace();
    }

    REQUIRE_THROWS_AS(sfe::get_current_exception_stacktrace(),
                      sfe::no_stacktrace_error);

    REQUIRE(trace1);
    REQUIRE(trace2);
    REQUIRE(trace3);
    REQUIRE(trace1 == trace2);
    REQUIRE(trace1 != trace3);
    REQUIRE(trace2 != trace3);
  };

  PassDifferentTypes(lambda_test);
}
