cmake_minimum_required(VERSION 3.10)

project(test_lib LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 20)

add_library(test_lib src/test.cpp)
target_include_directories(test_lib PUBLIC include)

add_subdirectory(logger)
target_link_libraries(test_lib logger)

add_subdirectory(tests)
