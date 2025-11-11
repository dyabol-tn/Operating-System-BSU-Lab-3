cmake_minimum_required(VERSION 3.10)
project(OS_Laboratory_Work_3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Lab3 
    main.cpp 
    marker.cpp 
    marker.h
)

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)
    include(GoogleTest)
    
    add_executable(Lab3_Tests
        tests/test_main.cpp
        tests/test_marker.cpp
        tests/test_utils.cpp
        marker.cpp
    )
    
    target_link_libraries(Lab3_Tests 
        GTest::gtest 
        GTest::gtest_main
    )
    
    target_include_directories(Lab3_Tests 
        PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    gtest_add_tests(TARGET Lab3_Tests)
endif()