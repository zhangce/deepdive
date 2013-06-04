
cmake_minimum_required(VERSION 2.8)
project(elementary)

######GTEST######
add_subdirectory(lib/gtest-1.6.0)
enable_testing(true)
include_directories(src test lib/gtest-1.6.0/include lib/gtest-1.6.0)
add_executable(ele_test test/test.cpp)
target_link_libraries(ele_test gtest gtest_main)
add_test(test ele_test)

######MAIN ENTRY######
add_executable(ele src/main.cpp)

