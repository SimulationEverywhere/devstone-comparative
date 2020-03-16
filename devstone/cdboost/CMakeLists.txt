cmake_minimum_required(VERSION 3.10)
project(devstone-cdboost)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_COMPILER "g++")

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED     OFF)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulators/cdboost/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(Boost COMPONENTS program_options thread unit_test_framework REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

## CDBoost
add_executable(cdboost-devstone src/cdboost-devstone.cpp events_devstone.txt)
target_include_directories(cdboost-devstone PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cdboost/include)
target_link_libraries(cdboost-devstone ${Boost_PROGRAM_OPTIONS_LIBRARY})
