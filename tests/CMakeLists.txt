include(CheckIncludeFileCXX)

include_directories(../contrib/catch2)
link_libraries(catch2)
link_libraries(sfe_basic)
link_libraries(dl)

# https://github.com/boostorg/stacktrace/issues/59

# check_include_file_cxx("backtrace.h" HAS_BACKTRACE_LIBRARY) # doesn't work
# if (HAS_BACKTRACE_LIBRARY)
if (NOT COMPILER_CLANG)

    link_libraries(backtrace dl)
    add_compile_definitions(BOOST_STACKTRACE_USE_BACKTRACE)
endif()

add_executable(test_libsfe_basic test_libsfe_basic.cpp)
add_executable(test_libsfe_preload test_libsfe_preload.cpp)


# if(SANITIZE_ENABLE)
#     target_compile_options(test_libsfe INTERFACE -fsanitize=address,undefined)
#     # target_link_libraries(test asan)
# endif()
