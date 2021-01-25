//===------------------------- cxa_exception.h ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//
//  This file implements the "Exception Handling APIs"
//  https://itanium-cxx-abi.github.io/cxx-abi/abi-eh.html
//
//===----------------------------------------------------------------------===//

#ifndef _CXA_EXCEPTION_H
#define _CXA_EXCEPTION_H

#include <exception>  // for std::unexpected_handler and std::terminate_handler
#include "cxxabi.h"
#include "unwind.h"

#include <sfe/stacktrace.hpp>

namespace __cxxabiv1 {

// TODO: hidden attribute

struct __cxa_exception {
#if defined(__LP64__) || defined(_WIN64) || defined(_LIBCXXABI_ARM_EHABI)
  // Now _Unwind_Exception is marked with __attribute__((aligned)),
  // which implies __cxa_exception is also aligned. Insert padding
  // in the beginning of the struct, rather than before unwindHeader.
  void* reserve;

  // This is a new field to support C++ 0x exception_ptr.
  // For binary compatibility it is at the start of this
  // struct which is prepended to the object thrown in
  // __cxa_allocate_exception.
  size_t referenceCount;
#endif

  //  Manage the exception object itself.
  std::type_info* exceptionType;
  void (*exceptionDestructor)(void*);
  std::unexpected_handler unexpectedHandler;
  std::terminate_handler terminateHandler;

  __cxa_exception* nextException;

  int handlerCount;

#if defined(_LIBCXXABI_ARM_EHABI)
  __cxa_exception* nextPropagatingException;
  int propagationCount;
#else
  int handlerSwitchValue;
  const unsigned char* actionRecord;
  const unsigned char* languageSpecificData;
  void* catchTemp;
  void* adjustedPtr;
#endif

#if !defined(__LP64__) && !defined(_WIN64) && !defined(_LIBCXXABI_ARM_EHABI)
  // This is a new field to support C++ 0x exception_ptr.
  // For binary compatibility it is placed where the compiler
  // previously adding padded to 64-bit align unwindHeader.
  size_t referenceCount;
#endif
  _Unwind_Exception unwindHeader;

  // my_stacktrace stacktrace;  // ! ABI break
};

// TODO: __cxa_dependent_exception
// TODO: static_assert's

}  // namespace __cxxabiv1

#endif  // _CXA_EXCEPTION_H
