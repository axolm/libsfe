#include <sfe/sfe.hpp>

#include <exception>
#include <mutex>
#include <unordered_map>

#include <cxxabi.h>
#include <dlfcn.h>

namespace {

std::unordered_map<void*, sfe::stacktrace> stacktrace_by_exc;
std::mutex map_mutex;
thread_local bool already_in_allocate_exception = false;

}  // namespace

namespace __cxxabiv1 {
extern "C" {

extern void* __cxa_allocate_exception(size_t thrown_size) throw() {
  if (already_in_allocate_exception) {  // for `bad_alloc`
    std::terminate();
  }
  already_in_allocate_exception = true;

  typedef void* (*cxa_allocate_exception_t)(size_t);
  auto orig_cxa_allocate_exception =
      (cxa_allocate_exception_t)dlsym(RTLD_NEXT, "__cxa_allocate_exception");

  void* user_obj_ptr = orig_cxa_allocate_exception(thrown_size);

  // void* user_obj_ptr =
  //     orig_cxa_allocate_exception(thrown_size + sizeof(sfe::stacktrace));
  // sfe::stacktrace* stacktrace_ptr =
  //     reinterpret_cast<sfe::stacktrace*>((char*)user_obj_ptr + thrown_size);

  auto trace = sfe::stacktrace(1, -1);

  {
    std::lock_guard<std::mutex> lg{map_mutex};
    auto it = stacktrace_by_exc.find(user_obj_ptr);
    if (it == stacktrace_by_exc.end()) {
      stacktrace_by_exc.emplace(user_obj_ptr, std::move(trace));
    } else {
      it->second = std::move(trace);
    }
  }

  return already_in_allocate_exception = false, user_obj_ptr;
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

extern const sfe::stacktrace* get_current_exception_stacktrace() {
  void* exc_raw_ptr = get_current_exception_raw_ptr();
  if (!exc_raw_ptr) {
    return nullptr;
  }

  std::lock_guard<std::mutex> lg{map_mutex};
  auto it = stacktrace_by_exc.find(exc_raw_ptr);
  if (it == stacktrace_by_exc.end()) {
    return nullptr;
  }
  return &it->second;
}

}  // namespace sfe
