#pragma once

#include <boost/stacktrace.hpp>

// TODO: use allocator without throws
using my_stacktrace = boost::stacktrace::stacktrace;
