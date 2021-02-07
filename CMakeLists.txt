project(sfe)

cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS  ON)
set(CMAKE_CXX_STANDARD 14)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set (COMPILER_CLANG 1)
endif()


add_compile_options("-g")
add_compile_options("-Wall" "-Wextra" "-Wpedantic" "-Werror")

if (COMPILER_CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# TODO: ...
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")

# set(CMAKE_MODULE_PATH              "${CMAKE_SOURCE_DIR}/cmake")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

include_directories(include)

add_subdirectory(contrib/catch2)
add_subdirectory(src/sfe)
add_subdirectory(tests)
