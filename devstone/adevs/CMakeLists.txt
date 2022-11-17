cmake_minimum_required(VERSION 3.10)
project(devstone-adevs)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../simulators/adevs-3.3/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(devstone src/devstone.cpp)

# Testing stuff
find_package(Boost COMPONENTS system filesystem unit_test_framework)
if (Boost_FOUND)
  add_definitions(-DBOOST_TEST_DYN_LINK)
  enable_testing()
  add_executable(test_devstone test/test_devstone.cpp)
  target_link_libraries(test_devstone ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
  add_test(NAME test_devstone COMMAND test_devstone)
else()
  message(STATUS "Boost not found. You won't be able to run tests for adevs.")
endif()
