cmake_minimum_required(VERSION 3.10)
project(devstone-cadmium)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_COMPILER "g++")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulators/cadmium/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(Boost COMPONENTS program_options thread unit_test_framework REQUIRED)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED     OFF)

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

## Cadmium dynamic, concurrent version
add_executable(cadmium-dynamic-conc-devstone
        src/cadmium-dynamic-devstone.cpp
        src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
        events_devstone.txt
        )

target_compile_definitions(cadmium-dynamic-conc-devstone
        PUBLIC CADMIUM_EXECUTE_CONCURRENT
        )
target_link_libraries(cadmium-dynamic-conc-devstone ${Boost_LIBRARIES} ${UTILOBJS})
