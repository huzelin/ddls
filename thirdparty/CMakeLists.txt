add_subdirectory(gtest)

# build zeromq
add_custom_target(zeromq ALL DEPENDS)
add_custom_command(TARGET zeromq
    PRE_BUILD
    COMMAND
    cd ${CMAKE_CURRENT_SOURCE_DIR}/zeromq/ && ./autogen.sh && ./configure && make -j 4
    COMMENT "Compile ZeroMQ")

add_library(libzeromq SHARED IMPORTED GLOBAL)
set_target_properties(libzeromq PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/zeromq/src/.libs/libzmq.so)
