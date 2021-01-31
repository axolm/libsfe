#pragma once

#include <sfe/stacktrace.hpp>

namespace sfe {

const stacktrace& get_current_exception_stacktrace();

// const stacktrace& get_stacktrace_from_exception(
//     std::exception_ptr exc_ptr);  // TODO: ???

class no_stacktrace_error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

}  // namespace sfe
