file(GLOB_RECURSE SOURCE
    "common/*.cc"
    "net/*.cc"
    "updater/*.cc"
    "table/*.cc"
    "frame/*.cc"
)
add_library(hpps SHARED ${SOURCE})
target_link_libraries(hpps zeromq)

if (USE_MPI)
target_link_libraries(hpps -lmpi)
endif()

