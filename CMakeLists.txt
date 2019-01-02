project(ddls)

cmake_minimum_required(VERSION 2.8)

option(test "Build all tests." ON)
enable_testing()
set(CMAKE_VERBOSE_MAKEFILE on)

include(${PROJECT_SOURCE_DIR}/cmake/Utils.cmake)
ddls_option(USE_HDFS "USE_HDFS" OFF)
set(CMAKE_INSTALL_PREFIX /opt/ddls/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -ftree-vectorize -std=c++11 -Wcpp -mavx -mavx2 -fopenmp -O2 -g")
if(USE_HDFS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_HDFS")
endif()

include_directories(
    ${PROJECT_BINARY_DIR}/
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/thirdparty/
    ${PROJECT_SOURCE_DIR}/gtest/
    ${PROJECT_SOURCE_DIR}/thirdparty/zeromq/include/
    ${PROJECT_SOURCE_DIR}/thirdparty/ompi/ompi/include/
    ${PROJECT_SOURCE_DIR}/thirdparty/hdfs/include/
)
link_directories(
    ${PROJECT_SOURCE_DIR}/thirdparty/hdfs/lib64/
)
add_subdirectory(thirdparty)

file(GLOB_RECURSE SOURCE
    "hpps/*.cc"
)
add_library(ddls SHARED ${SOURCE})
target_link_libraries(ddls libzeromq libompi)
if(USE_HDFS)
    target_link_libraries(ddls -lhdfs)
endif()
add_dependencies(ddls zeromq ompi)
install(TARGETS ddls DESTINATION lib)

add_subdirectory(test)
add_subdirectory(benchmark)
