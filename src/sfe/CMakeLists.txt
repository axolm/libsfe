add_library(sfe_basic SHARED sfe_basic.cpp)

# link_libraries(backtrace dl)
# add_compile_definitions(BOOST_STACKTRACE_USE_BACKTRACE)

add_library(sfe_preload SHARED sfe_preload.cpp)
add_library(sfe_preload_v2 SHARED sfe_preload_v2.cpp)

if(SANITIZE_ENABLE)
    # TODO
    # target_compile_options(sfe_shared INTERFACE -fsanitize=address)
    # target_link_libraries(sfe_shared asan)
endif()
