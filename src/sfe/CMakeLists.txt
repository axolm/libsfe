if (SANITIZE_ENABLE)
    add_compile_options(-fsanitize=address,undefined)
    # link_libraries(asan)
endif()

add_library(sfe_basic SHARED sfe_basic.cpp)

add_library(sfe_preload_v1 SHARED sfe_preload.cpp)
add_library(sfe_preload_v2 SHARED sfe_preload_v2.cpp)
add_library(sfe_preload_v3 SHARED sfe_preload_v3.cpp)
