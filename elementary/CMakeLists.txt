
cmake_minimum_required(VERSION 2.6)
project(elementary)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-O2 ")

######GTEST######
add_subdirectory(lib/gtest-1.6.0)
enable_testing(true)
include_directories(src test lib/gtest-1.6.0/include lib/gtest-1.6.0)
add_executable(ele_test src/common/timer.cpp src/fg/variable.cpp src/fg/factoraccessor.cpp src/io/varlenkv.cpp src/io/fixlenkv.cpp src/fg/variablescanner.cpp src/fg/factorgraph.cpp src/io/buffer.cpp src/io/driver_file.cpp src/io/frame.cpp src/io/scanner.cpp test/test.cpp)
target_link_libraries(ele_test gtest gtest_main)
add_test(test ele_test)

######MAIN ENTRY######
find_package (Threads)
add_executable(ele src/main.cpp)
target_link_libraries(ele ${CMAKE_THREAD_LIBS_INIT})
