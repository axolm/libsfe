project(sfe)

cmake_minimum_required(VERSION 3.8)

set(CMAKE_EXPORT_COMPILE_COMMANDS  ON)
set(CMAKE_CXX_STANDARD 17)

add_compile_options("-g")
add_compile_options("-Wall" "-Wextra" "-Wpedantic" "-Werror")

# TODO: ...
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")

# set(CMAKE_MODULE_PATH              "${CMAKE_SOURCE_DIR}/cmake")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

include_directories(include)

add_subdirectory(src/sfe)
add_subdirectory(tests)
