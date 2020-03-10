cmake_minimum_required(VERSION 3.15)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_COMPILER "g++")
project(devstone-adevs)
add_compile_options(-g)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulators/adevs/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)

add_executable(DEVStone src/DEVStone.cpp src/DummyAtomic.cpp src/DEVSWrapper.cpp)
