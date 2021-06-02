cmake_minimum_required(VERSION 3.10)
project(devstone-cadmium)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_COMPILER "g++")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulators/cadmium/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(Boost COMPONENTS program_options thread unit_test_framework REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

## Cadmium dynamic
add_executable(cadmium-dynamic-devstone
        src/cadmium-dynamic-devstone.cpp
        src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
        events_devstone.txt
        )
target_link_libraries(cadmium-dynamic-devstone
        ${Boost_PROGRAM_OPTIONS_LIBRARY} ${UTILOBJS}
        )
