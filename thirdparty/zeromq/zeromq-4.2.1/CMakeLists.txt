add_library(zeromq_static STATIC IMPORTED GLOBAL)
set_target_properties(zeromq_static PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/lib64/libzmq.a)

add_library(zeromq SHARED IMPORTED GLOBAL)
set_target_properties(zeromq PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/lib64/libzmq.so)

