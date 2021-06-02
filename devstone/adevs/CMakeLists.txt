cmake_minimum_required(VERSION 3.10)
project(devstone-adevs)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_COMPILER "g++")
add_compile_options(-O3)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulators/adevs/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS program_options unit_test_framework REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(DEVStone src/DEVStone.cpp src/DummyAtomic.cpp src/DEVSWrapper.cpp
        src/li.cpp src/hoMod.cpp src/hi src/ho.cpp src/seeder.cpp)
target_link_libraries(DEVStone ${Boost_PROGRAM_OPTIONS_LIBRARY})
