file(GLOB_RECURSE SOURCE
    "common/*.cc"
)
add_library(hpps SHARED ${SOURCE})
target_link_libraries(hpps zeromq)

if (USE_MPI)
target_link_libraries(hpps)
endif()

