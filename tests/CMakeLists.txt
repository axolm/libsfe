include(CheckIncludeFileCXX)

include_directories(../contrib/catch2)

if(SANITIZE_ENABLE)
    add_compile_options(-fsanitize=address,undefined)
    link_libraries(asan)
    link_libraries(ubsan)
endif()

link_libraries(catch2)
link_libraries(sfe_basic)
link_libraries(dl)
link_libraries(pthread)

if (NOT COMPILER_CLANG)
    # https://github.com/boostorg/stacktrace/issues/59
    # check_include_file_cxx("backtrace.h" HAS_BACKTRACE_LIBRARY) # doesn't work

    link_libraries(backtrace)
    add_compile_definitions(BOOST_STACKTRACE_USE_BACKTRACE)
endif()

add_executable(test_libsfe_basic test_libsfe_basic.cpp)
add_executable(test_libsfe_preload test_libsfe_preload.cpp)

add_executable(test_libsfe_preload_v3 test_libsfe_preload_v3.cpp)

# target_compile_definitions(test_libsfe_preload PUBLIC BOOST_STACKTRACE_DYN_LINK)
# target_link_libraries(test_libsfe_preload boost_stacktrace_backtrace dl)

# if(SANITIZE_ENABLE)
#     target_compile_options(test_libsfe INTERFACE -fsanitize=address,undefined)
#     # target_link_libraries(test asan)
# endif()
