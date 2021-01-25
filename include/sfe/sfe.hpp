#pragma once

#include <sfe/stacktrace.hpp>

namespace sfe {

const stacktrace& get_current_exception_stacktrace();

class no_trace_error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

}  // namespace sfe
