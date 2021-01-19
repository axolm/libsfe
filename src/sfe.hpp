#pragma once

#include "stacktrace.hpp"

namespace no_cxxabi /* TODO: name */ {

// 1: Return ptr?
// 2: Add type info to args?
// extern my_stacktrace __cxa_get_stacktrace_from_exception(
//     void* thrown_object) throw();

extern my_stacktrace __cxa_get_stacktrace_from_exception(
    void* thrown_object) throw();

}  // namespace no_cxxabi
