cmake_minimum_required(VERSION 2.4)

project(vx16)

set(HEADER_FILES
    include/vx16.h
)

set(TEST_SOURCE_FILES
    test/vx16test.cpp
)

include_directories(include)
add_executable(vx16test ${TEST_SOURCE_FILES} ${HEADER_FILES})
