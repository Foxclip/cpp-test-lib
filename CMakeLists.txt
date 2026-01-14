cmake_minimum_required(VERSION 3.10)

project(test_lib LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 20)

add_library(test_lib ${PROJECT_SOURCE_DIR}/src/test.cpp)
target_include_directories(test_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

if (NOT TARGET logger)
    # use local logger
    add_subdirectory(logger)
    target_link_libraries(test_lib PRIVATE logger)
else()
    # use externally provided logger
    get_target_property(LOGGER_INCLUDE_DIRS logger INCLUDE_DIRECTORIES)
    target_include_directories(test_lib PRIVATE ${LOGGER_INCLUDE_DIRS})
endif()

add_subdirectory(test_lib_tests)
