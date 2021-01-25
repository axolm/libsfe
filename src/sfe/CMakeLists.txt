set(LIBSFE_SOURCES
    sfe.cpp
)

add_library(sfe_shared SHARED ${LIBSFE_SOURCES})


# set(LIBSFE_HEADERS
#   ../../include/sfe/sfe.hpp
#   ../../include/sfe/stacktrace.hpp
# )
# add_library(sfe_shared SHARED ${LIBSFE_SOURCES} ${LIBSFE_HEADERS})

