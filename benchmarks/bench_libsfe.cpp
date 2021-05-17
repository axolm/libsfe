#include <catch.hpp>

#include <sfe/sfe.hpp>

#include <iostream>

// from google-benchmark
template <class Tp>
inline void DoNotOptimize(const Tp& value) {
  asm volatile("" : : "r,m"(value) : "memory");
}

TEST_CASE("Libsfe") {
  BENCHMARK("Simple throw") {
    try {
      throw std::runtime_error("error");
    } catch (const std::exception& exc) {
    }
  };
}
