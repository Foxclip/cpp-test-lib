add_executable(test_lib_tests
    main.cpp
)
target_link_libraries(test_lib_tests test_lib)
target_include_directories(test_lib_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
