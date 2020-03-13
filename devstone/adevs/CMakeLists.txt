cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_COMPILER "g++")
project(devstone-adevs)
add_compile_options(-g)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulators/adevs/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(Boost COMPONENTS program_options thread unit_test_framework REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})


set(CMAKE_CXX_STANDARD 17)

add_executable(DEVStone src/DEVStone.cpp src/DummyAtomic.cpp src/DEVSWrapper.cpp
        src/li.cpp src/hoMod.cpp src/hi.cpp src/hi.hpp src/ho.cpp src/seeder.cpp)

target_link_libraries(DEVStone ${Boost_PROGRAM_OPTIONS_LIBRARY})
