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

