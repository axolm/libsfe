#pragma once

#include <optional>

#include <sfe/stacktrace.hpp>

namespace sfe {
extern sfe::stacktrace get_current_exception_stacktrace();
}  // namespace sfe
