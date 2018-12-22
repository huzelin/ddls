project(hpps)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE "Release")

option(test "Build all tests." ON)
enable_testing()
set(CMAKE_VERBOSE_MAKEFILE on)

include(${PROJECT_SOURCE_DIR}/cmake/Utils.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -ftree-vectorize -std=c++11 -Wcpp -O2 -mavx -mavx2 -fopenmp -Wframe-larger-than=6963200")

include_directories(
    ${PROJECT_BINARY_DIR}/
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/thirdparty/
    ${PROJECT_SOURCE_DIR}/gtest/
    ${PROJECT_SOURCE_DIR}/thirdparty/zeromq/include/
    ${PROJECT_SOURCE_DIR}/thirdparty/ompi/ompi/include/
)

add_subdirectory(thirdparty)

file(GLOB_RECURSE SOURCE
    "hpps/*.cc"
)
add_library(hpps SHARED ${SOURCE})
target_link_libraries(hpps libzeromq libompi)
add_dependencies(hpps zeromq ompi)

add_subdirectory(test)
