project(sfe)

cmake_minimum_required(VERSION 3.8)

set(CMAKE_EXPORT_COMPILE_COMMANDS  ON)
add_compile_options("-g")
add_compile_options("-Wall" "-Wextra" "-Wpedantic" "-Werror")

set(CMAKE_CXX_STANDARD 17)

include_directories(include)


# TODO: ...
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")

# set(CMAKE_MODULE_PATH              "${CMAKE_SOURCE_DIR}/cmake")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(src/sfe)
add_subdirectory(tests)
