cmake_minimum_required(VERSION 3.10)
project(devstone-cadmium)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_COMPILER "g++")
add_compile_options(-O3)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulators/cadmium/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS program_options unit_test_framework REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

## Cadmium dynamic
add_executable(cadmium-dynamic-devstone src/cadmium-dynamic-devstone.cpp
        events_devstone.txt)
target_link_libraries(cadmium-dynamic-devstone
    ${Boost_PROGRAM_OPTIONS_LIBRARY} ${UTILOBJS})
