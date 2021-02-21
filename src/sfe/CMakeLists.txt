if (SANITIZE_ENABLE)
    add_compile_options(-fsanitize=address,undefined)
endif()

add_library(sfe_basic SHARED sfe_basic.cpp)
add_library(sfe_preload SHARED sfe_preload.cpp)
