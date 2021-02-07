#include <sfe/sfe.hpp>

#include "cxa_exception.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <typeinfo>

#include <cxxabi.h>

namespace __cxxabiv1 {

namespace {

// Round s up to next multiple of a.
inline size_t aligned_allocation_size(size_t s, size_t a) {
  return (s + a - 1) & ~(a - 1);
}

inline size_t cxa_exception_size_from_exception_thrown_size(size_t size) {
  return aligned_allocation_size(size + sizeof(__cxa_exception),
                                 alignof(__cxa_exception));
}

inline size_t get_cxa_exception_offset() {
  struct S {
  } __attribute__((aligned));

  constexpr size_t alignment = alignof(S);
  constexpr size_t excp_size = sizeof(__cxa_exception);
  constexpr size_t aligned_size =
      (excp_size + alignment - 1) / alignment * alignment;
  constexpr size_t offset = aligned_size - excp_size;
  static_assert((offset == 0 || alignof(_Unwind_Exception) < alignment),
                "offset is non-zero only if _Unwind_Exception isn't aligned");
  return offset;
}

inline void* thrown_object_from_cxa_exception(
    __cxa_exception* exception_header) {
  return static_cast<void*>(exception_header + 1);
}

inline __cxa_exception* cxa_exception_from_thrown_object(void* thrown_object) {
  return static_cast<__cxa_exception*>(thrown_object) - 1;
}
}  // namespace

extern "C" {

extern void* __cxa_allocate_exception(size_t thrown_size) throw() {
  size_t header_and_thrown_size =
      cxa_exception_size_from_exception_thrown_size(thrown_size);

  size_t header_offset = get_cxa_exception_offset();
  char* raw_buffer = (char*)malloc(header_offset + sizeof(sfe::stacktrace) +
                                   header_and_thrown_size);

  if (!raw_buffer) {
    std::terminate();
  }

  raw_buffer += header_offset;

  new (raw_buffer) sfe::stacktrace(1, -1);
  raw_buffer += sizeof(sfe::stacktrace);

  __cxa_exception* exception_header =
      static_cast<__cxa_exception*>((void*)(raw_buffer));
  ::memset(exception_header, 0, header_and_thrown_size);

  return thrown_object_from_cxa_exception(exception_header);
}

void __cxa_free_exception(void* thrown_object) throw() {
  char* raw_buffer = (char*)cxa_exception_from_thrown_object(thrown_object);
  raw_buffer -= sizeof(sfe::stacktrace);

  {
    using sfe::stacktrace;
    static_cast<sfe::stacktrace*>((void*)raw_buffer)->~stacktrace();
  }

  raw_buffer -= get_cxa_exception_offset();

  free((void*)raw_buffer);
}
}
}  // namespace __cxxabiv1

namespace sfe {

namespace {
using __cxxabiv1::__cxa_exception;
using __cxxabiv1::cxa_exception_from_thrown_object;

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
    // throw sfe::no_stacktrace_error(
    //     "cannot get trace because null exception ptr");
  }
  auto* exception_header = cxa_exception_from_thrown_object(exc_raw_ptr);
  return static_cast<sfe::stacktrace*>(
      (void*)((char*)exception_header - sizeof(sfe::stacktrace)));
}

}  // namespace sfe
