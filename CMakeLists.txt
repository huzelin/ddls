project(hpps)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE "Release")

option(test "Build all tests." ON)
enable_testing()
set(CMAKE_VERBOSE_MAKEFILE on)

include(${PROJECT_SOURCE_DIR}/cmake/Utils.cmake)

#########################################################
hpps_option(USE_MPI "USE_MPI" OFF)

message(STATUS "USE_MPI=${USE_MPI}")

if (USE_MPI)
set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -DUSE_MPI")
# You can reset MPI dir here
include_directories(${MPI_PATH}/include)
link_directories(${MPI_PATH}/lib)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -ftree-vectorize -std=c++11 -Wcpp -O2 -mavx -mavx2 -fopenmp -Wframe-larger-than=6963200")

include_directories(
  ${PROJECT_BINARY_DIR}/
  ${PROJECT_SOURCE_DIR}/
  ${PROJECT_SOURCE_DIR}/thirdparty/
  ${PROJECT_SOURCE_DIR}/gtest/
  ${PROJECT_SOURCE_DIR}/thirdparty/zeromq/zeromq-4.2.1/include/
)

add_subdirectory(thirdparty)
add_subdirectory(hpps)
add_subdirectory(tests)
