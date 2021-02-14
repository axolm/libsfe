#pragma once

#include <sfe/stacktrace.hpp>

namespace sfe {

extern const stacktrace* get_current_exception_stacktrace();

}  // namespace sfe
