// #define _GNU_SOURCE

#include "sfe.hpp"

#include <dlfcn.h>  // TODO
#include <stdio.h>

// TODO
#include <cstdio>
#include <cstdlib>
#include <typeinfo>

#include <string.h>   // for memset
#include <exception>  // for std::terminate

#include "cxa_exception.h"

#include <unordered_map>

// Just for work example
static std::unordered_map<uintptr_t, size_t> requests;
static size_t nbytes_requested_for_alloc(void* addr) {
  return requests.at((uintptr_t)addr);
}
static inline void* custom_malloc(size_t n) {
  void* res = malloc(n);
  requests[(uintptr_t)res] = n;
  return res;
}
auto custom_free = free;

// auto custom_malloc = malloc;
// auto custom_free = free;
// #include "cxxabi_copypaste/fallback_malloc.h" // TODO

// TODO: attributes
namespace __cxxabiv1 {

// Round s up to next multiple of a.
static inline size_t aligned_allocation_size(size_t s, size_t a) {
  return (s + a - 1) & ~(a - 1);
}

size_t cxa_exception_size_from_exception_thrown_size(size_t size) {
  // TODO: align
  return aligned_allocation_size(size + sizeof(__cxa_exception),
                                 alignof(__cxa_exception)) +
         sizeof(my_stacktrace);
}

static size_t get_cxa_exception_offset() {
  struct S {
  } __attribute__((aligned));

  // Compute the maximum alignment for the target machine.
  constexpr size_t alignment = alignof(S);
  constexpr size_t excp_size = sizeof(__cxa_exception);
  constexpr size_t aligned_size =
      (excp_size + alignment - 1) / alignment * alignment;
  constexpr size_t offset = aligned_size - excp_size;
  static_assert((offset == 0 || alignof(_Unwind_Exception) < alignment),
                "offset is non-zero only if _Unwind_Exception isn't aligned");
  return offset;
}

static inline void* thrown_object_from_cxa_exception(
    __cxa_exception* exception_header) {
  return static_cast<void*>(exception_header + 1);
}

extern "C" {

extern void* __cxa_allocate_exception(size_t thrown_size) throw() {
  size_t actual_size =
      cxa_exception_size_from_exception_thrown_size(thrown_size);

  size_t header_offset = get_cxa_exception_offset();
  char* raw_buffer = (char*)custom_malloc(header_offset + actual_size);
  if (NULL == raw_buffer) std::terminate();
  __cxa_exception* exception_header =
      static_cast<__cxa_exception*>((void*)(raw_buffer + header_offset));
  ::memset(exception_header, 0, actual_size);

  // exception_header->stacktrace = my_stacktrace();


  auto* stacktrace_addr = reinterpret_cast<my_stacktrace*>((char*)exception_header + actual_size)-1;
  new (stacktrace_addr) my_stacktrace(1, -1);
  // fprintf(stderr, "addr = %lu\n", stacktrace_addr);

  return thrown_object_from_cxa_exception(exception_header);
}
}

//  Utility routines
static inline __cxa_exception* cxa_exception_from_thrown_object(
    void* thrown_object) {
  return static_cast<__cxa_exception*>(thrown_object) - 1;
}

void __cxa_free_exception(void* thrown_object) throw() {
  size_t header_offset = get_cxa_exception_offset();
  char* raw_buffer =
      ((char*)cxa_exception_from_thrown_object(thrown_object)) - header_offset;

  // TODO: stacktrace destructor

  custom_free((void*)raw_buffer);
}

}  // namespace __cxxabiv1

namespace no_cxxabi {

using __cxxabiv1::__cxa_exception;

static inline __cxa_exception* cxa_exception_from_thrown_object(
    void* thrown_object) {
  return static_cast<__cxa_exception*>(thrown_object) - 1;
}

// TODO
// 1. pass as second argument `sizeof(Exception)` - incorrect
// 2. get sizeof from exception_header->exceptionType  - maybe
// 3. get from allocator size info - dirty hack
// We can use own allocator and always known this size
my_stacktrace* get_stacktrace_addr(void* thrown_object) {
  auto* exception_header = cxa_exception_from_thrown_object(thrown_object);

  auto nbytes = nbytes_requested_for_alloc(exception_header);

  return reinterpret_cast<my_stacktrace*>((char*)exception_header + nbytes) - 1;
}

extern my_stacktrace __cxa_get_stacktrace_from_exception(
    void* thrown_object) throw() {
  auto* stacktrace_addr = get_stacktrace_addr(thrown_object);
  // fprintf(stderr, "addr2 = %lu\n", stacktrace_addr);
  return *stacktrace_addr;
}

}  // namespace no_cxxabi
