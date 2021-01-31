add_library(catch_main ../contrib/catch2/catch_main.cpp)

add_executable(tests tests.cpp)
target_include_directories(tests PUBLIC ../contrib/catch2)
target_link_libraries(tests catch_main)
target_link_libraries(tests dl sfe_shared)

# add_executable(test_asan tests.cpp)
# target_include_directories(test_asan PUBLIC ../contrib/catch2)
# target_link_libraries(test_asan catch_main)
# target_link_libraries(test_asan dl sfe_shared)
# target_compile_options(test_asan INTERFACE -fsanitize=address,undefined)
