add_executable(test
    ${PROJECT_SOURCE_DIR}/tests/main.cpp
)
target_link_libraries(test test_lib)
target_include_directories(test PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
