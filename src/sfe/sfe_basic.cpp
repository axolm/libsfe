#include <sfe/sfe.hpp>

namespace sfe {
extern const sfe::stacktrace* get_current_exception_stacktrace() {
  return nullptr;
}

extern std::optional<sfe::stacktrace> get_current_exception_stacktrace_v3() {
  return std::nullopt;
}
}  // namespace sfe
