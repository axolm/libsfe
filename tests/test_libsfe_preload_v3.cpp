#include <catch.hpp>

#include <sfe/sfe.hpp>

#include <atomic>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <regex>
#include <stdexcept>
#include <thread>
#include <utility>

namespace {
template <typename T>
std::string ToStr(T&& arg) {
  std::ostringstream ss;
  ss << std::forward<T>(arg);
  REQUIRE(ss);
  return std::move(ss).str();
}

void CheckSymbolsInLambda(const sfe::stacktrace& stacktrace) {
#ifdef BOOST_STACKTRACE_USE_BACKTRACE
  const auto& vector_ref = stacktrace.as_vector();

  {
    static const std::regex kRegex{
        R"(^operator\(\)<.*>.*test_libsfe_preload_v3\.cpp:\d+$)"};
    REQUIRE(std::regex_match(ToStr(vector_ref[0]), kRegex));
  }

  {
    static const std::regex kRegex{
        R"(^PassDifferentTypes.*test_libsfe_preload_v3\.cpp:\d+$)"};
    REQUIRE(std::regex_match(ToStr(vector_ref[1]), kRegex));
  }

  auto str = ToStr(stacktrace);
  REQUIRE(str.find(" main ") != std::string::npos);
  REQUIRE(str.find("libc") != std::string::npos);

#else
  std::ignore = stacktrace;
#endif
}

void CheckSymbolsMultithread(const sfe::stacktrace& stacktrace) {
#ifdef BOOST_STACKTRACE_USE_BACKTRACE
  auto str = ToStr(stacktrace);
  REQUIRE(str.find("test_libsfe_preload_v3.cpp") != std::string::npos);
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
}  // namespace

TEST_CASE("Basic") {
  auto lambda_test = [](auto&& arg) {
    using ExcType = std::decay_t<decltype(arg)>;
    try {
      throw std::forward<ExcType>(arg);
    } catch (ExcType& exc) {
      auto trace_opt = sfe::get_current_exception_stacktrace_v3();
      REQUIRE(trace_opt);
      REQUIRE(*trace_opt);
      CheckSymbolsInLambda(*trace_opt);
      REQUIRE(*trace_opt == *sfe::get_current_exception_stacktrace_v3());
    }

    REQUIRE(!sfe::get_current_exception_stacktrace_v3());
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
      REQUIRE(*sfe::get_current_exception_stacktrace_v3());
      ptr = std::current_exception();
      REQUIRE(*sfe::get_current_exception_stacktrace_v3());
    }

    try {
      std::rethrow_exception(ptr);
    } catch (...) {
      CheckSymbolsInLambda(*sfe::get_current_exception_stacktrace_v3());
    }
    REQUIRE(!sfe::get_current_exception_stacktrace_v3());
  };

  PassDifferentTypes(lambda_test);
}

TEST_CASE("DeepThrow") {
  auto lambda_test = [](auto&& arg) {
    using ExcType = std::decay_t<decltype(arg)>;

    sfe::stacktrace trace1, trace2, trace3;

    REQUIRE(!sfe::get_current_exception_stacktrace_v3());
    try {
      try {
        try {
          throw std::forward<ExcType>(arg);
        } catch (ExcType& exc) {
          trace1 = *sfe::get_current_exception_stacktrace_v3();
          throw;
        }
      } catch (ExcType& exc) {
        trace2 = *sfe::get_current_exception_stacktrace_v3();
        throw exc;
      }
    } catch (...) {
      trace3 = *sfe::get_current_exception_stacktrace_v3();
    }

    REQUIRE(!sfe::get_current_exception_stacktrace_v3());

    REQUIRE(trace1);
    REQUIRE(trace2);
    REQUIRE(trace3);
    REQUIRE(trace1 == trace2);
    REQUIRE(trace1 != trace3);
    REQUIRE(trace2 != trace3);

    CheckSymbolsInLambda(trace1);
    CheckSymbolsInLambda(trace2);
    CheckSymbolsInLambda(trace3);
  };

  PassDifferentTypes(lambda_test);
}

TEST_CASE("Multithread test") {
  static const size_t kThreadsCount = 10;
  std::atomic_bool start{false};

  auto work = [&start]() {
    while (not start.load())
      ;  // spin lock
    for (size_t i = 0; i < 30; ++i) {
      REQUIRE(!sfe::get_current_exception_stacktrace_v3());
      try {
        throw std::runtime_error("some error");
      } catch (const std::exception& exc) {
        CheckSymbolsMultithread(*sfe::get_current_exception_stacktrace_v3());
      }
    }
  };

  std::vector<std::thread> threads;
  threads.reserve(kThreadsCount);
  for (size_t i = 0; i < kThreadsCount; ++i) {
    threads.emplace_back(work);
  }
  start.store(true);
  for (auto& t : threads) {
    t.join();
  }
}

namespace {

std::mt19937& Generator() {
  // Seed is not significant for error injection
  static thread_local std::mt19937 kGen(
      std::hash<std::thread::id>()(std::this_thread::get_id()));
  return kGen;
}

}  // namespace

TEST_CASE("Multithread test with queue") {
  static const size_t kThreadsCount = 10;
  std::atomic_bool start{false};

  std::queue<std::exception_ptr> q;
  std::mutex qmutex;

  auto push = [&q, &qmutex] {
    try {
      throw std::runtime_error("some error");
    } catch (const std::exception& exc) {
      auto exc_ptr = std::current_exception();
      {
        std::lock_guard<std::mutex> lg{qmutex};
        q.push(exc_ptr);
      }
    }
  };

  auto pop = [&q, &qmutex] {
    std::exception_ptr exc_ptr;
    {
      std::lock_guard<std::mutex> lg{qmutex};
      if (q.empty()) {
        return;
      }
      exc_ptr = q.front();
      q.pop();
    }
    try {
      std::rethrow_exception(exc_ptr);
    } catch (const std::exception& exc) {
      CheckSymbolsMultithread(*sfe::get_current_exception_stacktrace_v3());
    }
  };

  auto work = [&push, &pop, &start]() {
    while (not start.load())
      ;  // spin lock
    for (size_t i = 0; i < 50; ++i) {
      REQUIRE(!sfe::get_current_exception_stacktrace_v3());
      if (std::uniform_int_distribution<int>(0, 1)(Generator())) {
        push();
      } else {
        pop();
      }
    }
  };

  std::vector<std::thread> threads;
  threads.reserve(kThreadsCount);
  for (size_t i = 0; i < kThreadsCount; ++i) {
    threads.emplace_back(work);
  }
  start.store(true);
  for (auto& t : threads) {
    t.join();
  }

  while (not q.empty()) {
    pop();
  }

  push();
  pop();
}
