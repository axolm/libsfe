#include <sfe/sfe.hpp>

#include "cxa_exception.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <typeinfo>
#include <unordered_map>

#include <cxxabi.h>

#include <dlfcn.h>

namespace {
// TODO: Make thread safe. `thread_local` will not work
std::unordered_map<void*, sfe::stacktrace*> stacktrace_by_exc;
thread_local bool already_in_allocate_exception = false;
}  // namespace

namespace __cxxabiv1 {

extern "C" {

typedef void* (*orig_cxa_allocate_exception_t)(size_t);

extern void* __cxa_allocate_exception(size_t thrown_size) throw() {
  if (already_in_allocate_exception) {  // for `bad_alloc`
    std::terminate();
  }
  already_in_allocate_exception = true;

  orig_cxa_allocate_exception_t orig_cxa_allocate_exception =
      (orig_cxa_allocate_exception_t)dlsym(RTLD_NEXT,
                                           "__cxa_allocate_exception");

  void* user_obj_ptr =
      orig_cxa_allocate_exception(thrown_size + sizeof(sfe::stacktrace));

  sfe::stacktrace* stacktrace_ptr =
      reinterpret_cast<sfe::stacktrace*>((char*)user_obj_ptr + thrown_size);

  new (stacktrace_ptr) sfe::stacktrace(1, -1);

  stacktrace_by_exc[user_obj_ptr] = stacktrace_ptr;

  return already_in_allocate_exception = false, user_obj_ptr;
}
}
}  // namespace __cxxabiv1

namespace sfe {

namespace {

// TODO
inline void* get_current_exception_raw_ptr() {
  // https://nda.ya.ru/t/ngbQH_OG3hhH2U
  auto exc_ptr = std::current_exception();
  void* exc_raw_ptr = *static_cast<void**>((void*)&exc_ptr);  // UB?
  return exc_raw_ptr;
}

}  // namespace

extern const sfe::stacktrace* get_current_exception_stacktrace() {
  void* exc_raw_ptr = get_current_exception_raw_ptr();
  if (!exc_raw_ptr) {
    return nullptr;
  }

  // returns nullptr if no
  return stacktrace_by_exc[exc_raw_ptr];
}

}  // namespace sfe
