#pragma once

#include "sfe.hpp"
#include "stacktrace.hpp"

namespace std {

// TODO:
// 1. exception_ptr?
// 2. return `const my_stacktrace&`?
// 3. error handling

template <typename Exception>
my_stacktrace get_stacktrace_from_exception(const Exception& exc) {
  return no_cxxabi::__cxa_get_stacktrace_from_exception((void*)&exc);
}


}  // namespace std
