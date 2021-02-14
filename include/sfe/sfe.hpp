#pragma once

#include <optional>

#include <sfe/stacktrace.hpp>

namespace sfe {

extern const stacktrace* get_current_exception_stacktrace();

extern std::optional<sfe::stacktrace> get_current_exception_stacktrace_v3();

}  // namespace sfe
