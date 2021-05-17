#include <sfe/sfe.hpp>

#include <cstddef>
#include <exception>
#include <mutex>
#include <unordered_map>
#include <utility>

#include <dlfcn.h>

namespace {

const size_t kStacktraceDumpSize = 4096;

std::unordered_map<void*, const char*> stacktrace_dump_by_exc;
std::mutex mutex;

}  // namespace

namespace __cxxabiv1 {
extern "C" {

extern void* __cxa_allocate_exception(size_t thrown_size) throw() {
  static thread_local bool already_in_allocate_exception = false;
  if (std::exchange(already_in_allocate_exception, true)) {  // for `bad_alloc`
    std::terminate();
  }

  typedef void* (*cxa_allocate_exception_t)(size_t);
  static auto orig_cxa_allocate_exception =
      (cxa_allocate_exception_t)dlsym(RTLD_NEXT, "__cxa_allocate_exception");

  static constexpr size_t kAlign = alignof(std::max_align_t);
  thrown_size = (thrown_size + kAlign - 1) & (~(kAlign - 1));  // round up

  void* user_obj_ptr =
      orig_cxa_allocate_exception(thrown_size + kStacktraceDumpSize);

  char* stacktrace_dump_ptr = ((char*)user_obj_ptr + thrown_size);

  // TODO: full dynamic serialization
  boost::stacktrace::safe_dump_to(1, stacktrace_dump_ptr, kStacktraceDumpSize);
  {
    std::lock_guard<std::mutex> lg{mutex};
    stacktrace_dump_by_exc[user_obj_ptr] = stacktrace_dump_ptr;
  }

  return already_in_allocate_exception = false, user_obj_ptr;
}

// TODO: Not called in libc++
// So the `stacktrace_dump_by_exc` is not cleared. That's not fatal
extern void __cxa_free_exception(void* thrown_object) throw() {
  static thread_local bool already_in_free_exception = false;
  if (std::exchange(already_in_free_exception, true)) {
    std::terminate();
  }

  typedef void (*cxa_free_exception_t)(void*);
  static auto orig_cxa_free_exception =
      (cxa_free_exception_t)dlsym(RTLD_NEXT, "__cxa_free_exception");
  orig_cxa_free_exception(thrown_object);

  {
    std::lock_guard<std::mutex> lg{mutex};
    stacktrace_dump_by_exc.erase(thrown_object);
  }

  already_in_free_exception = false;
}
}
}  // namespace __cxxabiv1

namespace sfe {

namespace {

inline void* get_current_exception_raw_ptr() {
  // https://github.com/gcc-mirror/gcc/blob/16e2427f50c208dfe07d07f18009969502c25dc8/libstdc%2B%2B-v3/libsupc%2B%2B/eh_ptr.cc#L147
  auto exc_ptr = std::current_exception();
  void* exc_raw_ptr = *static_cast<void**>((void*)&exc_ptr);  // TODO
  return exc_raw_ptr;
}

}  // namespace

extern sfe::stacktrace get_current_exception_stacktrace() {
  static const sfe::stacktrace kEmpty{0, 0};

  void* exc_raw_ptr = get_current_exception_raw_ptr();
  if (!exc_raw_ptr) {
    return kEmpty;
  }

  const char* stacktrace_dump_ptr;
  {
    std::lock_guard<std::mutex> lg{mutex};
    auto it = stacktrace_dump_by_exc.find(exc_raw_ptr);
    if (it == stacktrace_dump_by_exc.end()) {
      return kEmpty;
    }
    stacktrace_dump_ptr = it->second;
  }

  return boost::stacktrace::stacktrace::from_dump(stacktrace_dump_ptr,
                                                  kStacktraceDumpSize);
}

}  // namespace sfe
