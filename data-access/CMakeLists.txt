cmake_minimum_required(VERSION 2.8)

project(data-access)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILECMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O2 -Wall -g -ggdb")
set(CMAKE_BUILD_TYPE "Debug")
set(_GLIBCXX_USE_NANOSLEEP ON)
add_definitions(
    -std=c++17
)

set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC_DIR "${PROJECT_DIR}/src")
set(BOOST_INCLUDE_DIR "/home/work/sofa-pbrpc/sofa-pbrpc/thirdparty/boost_1_57_0/")
set(COMMON_DIR "${PROJECT_DIR}/common")
set(PROTO_DIR "${PROJECT_DIR}/proto")
set(TEST_DIR "${PROJECT_DIR}/test")
set(THIRDPARTY_DIR "${PROJECT_DIR}/thirdparty")
set(THIRDPARTY_INCLUDE_DIR "${THIRDPARTY_DIR}/include")
set(THIRDPARTY_BIN_DIR "${THIRDPARTY_DIR}/bin")
set(THIRDPARTY_LIB_DIR "${THIRDPARTY_DIR}/lib")

add_subdirectory(${COMMON_DIR})
add_subdirectory(${TEST_DIR})
add_subdirectory(${PROTO_DIR})
add_subdirectory(${SRC_DIR})

