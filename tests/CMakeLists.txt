add_library(catch_main ../contrib/catch2/catch_main.cpp)

add_executable(test test.cpp)
target_include_directories(test PUBLIC ../contrib/catch2)

target_link_libraries(test catch_main)
target_link_libraries(test dl)
target_link_libraries(test sfe_shared)


if(SANITIZE_ENABLE)
    target_compile_options(test INTERFACE -fsanitize=address,undefined)
    # target_link_libraries(test asan)
endif()
