#include <sfe/sfe.hpp>

namespace sfe {
extern sfe::stacktrace get_current_exception_stacktrace() {
  static const sfe::stacktrace kEmpty{0, 0};
  return kEmpty;
}
}  // namespace sfe
