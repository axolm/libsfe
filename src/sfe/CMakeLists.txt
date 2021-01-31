add_library(sfe_shared SHARED sfe.cpp)

if(SANITIZE_ENABLE)
    # TODO
    # target_compile_options(sfe_shared INTERFACE -fsanitize=address)
    # target_link_libraries(sfe_shared asan)
endif()
