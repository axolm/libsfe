#include <catch.hpp>

#include <sfe/sfe.hpp>

#include <iostream>
#include <stdexcept>
#include <utility>

#include <regex>

void CheckNames(const sfe::stacktrace& stacktrace) {
#ifdef BOOST_STACKTRACE_USE_BACKTRACE
  const auto& vector_ref = stacktrace.as_vector();

  auto to_str = [](const auto& arg) {
    std::ostringstream ss;
    ss << arg;
    REQUIRE(ss);
    return std::move(ss).str();
  };

  {
    static const std::regex kRegex{
        R"(^operator\(\)<.*>.*test_libsfe_preload\.cpp:\d+$)"};
    REQUIRE(std::regex_match(to_str(vector_ref[0]), kRegex));
  }

  {
    static const std::regex kRegex{
        R"(^PassDifferentTypes.*test_libsfe_preload\.cpp:\d+$)"};
    REQUIRE(std::regex_match(to_str(vector_ref[1]), kRegex));
  }

  auto str = to_str(stacktrace);
  REQUIRE(str.find(" main ") != std::string::npos);
  REQUIRE(str.find("libc") != std::string::npos);

#else
  std::ignore = stacktrace;
#endif
}

template <typename Func>
  __attribute__((noinline))  // For showing in trace
void PassDifferentTypes(Func && func) {
  func(std::runtime_error("hello"));
  // func(std::exception());
  func(42);
  // func(true);
  // func("abacaba");
  func(sfe::stacktrace{});
}

TEST_CASE("Basic") {
  auto lambda_test = [](auto&& arg) {
    using ExcType = std::decay_t<decltype(arg)>;
    try {
      throw std::forward<ExcType>(arg);
    } catch (ExcType& exc) {
      auto* trace_ptr = sfe::get_current_exception_stacktrace();
      REQUIRE(trace_ptr);
      REQUIRE(*trace_ptr);
      CheckNames(*trace_ptr);
      REQUIRE(*trace_ptr == *sfe::get_current_exception_stacktrace());
    }

    REQUIRE(!sfe::get_current_exception_stacktrace());
    // REQUIRE_THROWS_AS(sfe::get_current_exception_stacktrace(),
    //                   sfe::no_stacktrace_error);
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
      REQUIRE(*sfe::get_current_exception_stacktrace());
      ptr = std::current_exception();
      REQUIRE(*sfe::get_current_exception_stacktrace());
    }

    try {
      std::rethrow_exception(ptr);
    } catch (...) {
      auto trace = *sfe::get_current_exception_stacktrace();
      CheckNames(trace);
    }
    REQUIRE(!sfe::get_current_exception_stacktrace());
    // REQUIRE_THROWS_AS(*sfe::get_current_exception_stacktrace(),
    //                   sfe::no_stacktrace_error);
  };

  PassDifferentTypes(lambda_test);
}

TEST_CASE("DeepThrow") {
  auto lambda_test = [](auto&& arg) {
    using ExcType = std::decay_t<decltype(arg)>;

    sfe::stacktrace trace1, trace2, trace3;

    REQUIRE(!sfe::get_current_exception_stacktrace());
    // REQUIRE_THROWS_AS(sfe::get_current_exception_stacktrace(),
    //                   sfe::no_stacktrace_error);
    try {
      try {
        try {
          throw std::forward<ExcType>(arg);
        } catch (ExcType& exc) {
          trace1 = *sfe::get_current_exception_stacktrace();
          throw;
        }
      } catch (ExcType& exc) {
        trace2 = *sfe::get_current_exception_stacktrace();
        throw exc;
      }
    } catch (...) {
      trace3 = *sfe::get_current_exception_stacktrace();
    }

    REQUIRE(!sfe::get_current_exception_stacktrace());
    // REQUIRE_THROWS_AS(sfe::get_current_exception_stacktrace(),
    //                   sfe::no_stacktrace_error);

    REQUIRE(trace1);
    REQUIRE(trace2);
    REQUIRE(trace3);
    REQUIRE(trace1 == trace2);
    REQUIRE(trace1 != trace3);
    REQUIRE(trace2 != trace3);

    // if (std::is_same<ExcType, std::runtime_error>::value) {
    //   std::cerr << "TRACE = " << trace1;
    // }

    CheckNames(trace1);
    CheckNames(trace2);
    CheckNames(trace3);
  };

  PassDifferentTypes(lambda_test);
}

TEST_CASE("Multithread test") {
  // TODO
}
