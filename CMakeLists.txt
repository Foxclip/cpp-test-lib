set(TEST_LIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/test_lib")
set(TEST_LIB_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(TEST_LIB_HEADER_FILES
    "${TEST_LIB_INCLUDE_DIR}/test.h"
)
set(TEST_LIB_SOURCE_FILES
    "${TEST_LIB_SOURCE_DIR}/test.cpp"
)
add_library(test_lib ${TEST_LIB_HEADER_FILES} ${TEST_LIB_SOURCE_FILES})
source_group(TREE ${TEST_LIB_INCLUDE_DIR} PREFIX "Header Files" FILES ${TEST_LIB_HEADER_FILES})
source_group(TREE ${TEST_LIB_SOURCE_DIR} PREFIX "Source Files" FILES ${TEST_LIB_SOURCE_FILES})
set_property(TARGET test_lib PROPERTY FOLDER "test_lib")

target_include_directories(test_lib PRIVATE "${TEST_LIB_INCLUDE_DIR}")
target_link_libraries(test_lib PUBLIC common_lib)
#set_property(TARGET test_lib PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

set(TEST_LIB_EXAMPLE_HEADER_FILES
    "${TEST_LIB_INCLUDE_DIR}/example/test_list_3.h"
    "${TEST_LIB_INCLUDE_DIR}/example/test_module_1.h"
    "${TEST_LIB_INCLUDE_DIR}/example/test_module_2.h"
    "${TEST_LIB_INCLUDE_DIR}/example/test_module_3.h"
)
set(TEST_LIB_EXAMPLE_SOURCE_FILES
    "${TEST_LIB_SOURCE_DIR}/example/test_list_3.cpp"
    "${TEST_LIB_SOURCE_DIR}/example/test_module_1.cpp"
    "${TEST_LIB_SOURCE_DIR}/example/test_module_2.cpp"
    "${TEST_LIB_SOURCE_DIR}/example/test_module_3.cpp"
    "${TEST_LIB_SOURCE_DIR}/example/main.cpp"
)
add_executable(test_lib_example ${TEST_LIB_EXAMPLE_HEADER_FILES} ${TEST_LIB_EXAMPLE_SOURCE_FILES})
source_group(TREE "${TEST_LIB_INCLUDE_DIR}/example" PREFIX "Header Files" FILES ${TEST_LIB_EXAMPLE_HEADER_FILES})
source_group(TREE "${TEST_LIB_SOURCE_DIR}/example" PREFIX "Source Files" FILES ${TEST_LIB_EXAMPLE_SOURCE_FILES})
set_property(TARGET test_lib_example PROPERTY FOLDER "test_lib")

target_include_directories(test_lib_example PRIVATE "${TEST_LIB_INCLUDE_DIR}")
target_link_libraries(test_lib_example PUBLIC test_lib)
