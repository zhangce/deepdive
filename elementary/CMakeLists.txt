
cmake_minimum_required(VERSION 2.6)
project(elementary)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

#set(CMAKE_BUILD_TYPE Release)
# We cannot use -O3 becaues of -ftree-vectorize --- #TODO: CHECKING REASONS
set(CMAKE_CXX_FLAGS "-O2 -DHAVE_NETDB_H=1  -std=c++0x -ffast-math -finline-functions -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-partial-pre -fipa-cp-clone -fvect-cost-model")

######GTEST######
add_subdirectory(lib/gtest-1.6.0)
enable_testing(true)
include_directories(src test lib/gtest-1.6.0/include lib/gtest-1.6.0)

add_custom_command(
  OUTPUT /tmp/deepdive
  COMMAND mkdir -p /tmp/deepdive;
)

add_custom_command(
  OUTPUT /tmp/deepdive/variables.tsv
  COMMAND python ${CMAKE_SOURCE_DIR}/test/prepare/prepare.py -a lda -e ${CMAKE_SOURCE_DIR}/../testdata/lda/
)

ADD_CUSTOM_TARGET(PrepareTest ALL echo
  DEPENDS /tmp/deepdive /tmp/deepdive/variables.tsv
)

if(NOT WITHTHRIFT)

  ####TEST####
  message(STATUS "Build without THRIFT SUPPORT")

  add_executable(ele_test src/fg/modelaccessor.cpp src/fg/variable.cpp src/fg/factoraccessor.cpp src/io/varlenkv.cpp src/io/fixlenkv.cpp src/fg/variablescanner.cpp src/fg/factorgraph.cpp src/io/buffer.cpp src/io/driver_file.cpp src/io/frame.cpp src/io/scanner.cpp test/test.cpp)
  target_link_libraries(ele_test gtest gtest_main)
  add_test(test ele_test)
  target_link_libraries(ele_test -lrt)

  ######MAIN ENTRY######
  find_package (Threads)
  add_executable(ele src/main.cpp)
  target_link_libraries(ele ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(ele -lrt)

else()

  message(STATUS "Build with THRIFT SUPPORT")

  ######THRIFT######
  find_package(Thrift REQUIRED)

  if(Thrift_FOUND)
    message (STATUS "Thrift_INCLUDE_DIR=${Thrift_INCLUDE_DIR}")
    message (STATUS "Thrift_LIB=${Thrift_LIB}")
  else()
    if(NOT Thrift_INCLUDE_DIR OR NOT Thrift_LIB)
      message(FATAL_ERROR "Thrift not found! Set Thrift_INCLUDE_DIR and Thrift_LIB")	
    else()
      message (STATUS "Thrift_INCLUDE_DIR=${Thrift_INCLUDE_DIR}")
      message (STATUS "Thrift_LIB=${Thrift_LIB}")
    endif()
  endif()

  ####TEST####
  include_directories("${Thrift_INCLUDE_DIR}")
  add_executable(ele_test src/io/hbase/Hbase.cpp src/io/hbase/hbase_constants.cpp src/io/hbase/hbase_types.cpp src/io/driver_hbase.cpp src/fg/modelaccessor.cpp src/fg/variable.cpp src/fg/factoraccessor.cpp src/io/varlenkv.cpp src/io/fixlenkv.cpp src/fg/variablescanner.cpp src/fg/factorgraph.cpp src/io/buffer.cpp src/io/driver_file.cpp src/io/frame.cpp src/io/scanner.cpp test/test.cpp)
  add_test(test ele_test)
  target_link_libraries(ele_test -lrt)
  target_link_libraries(ele_test gtest gtest_main) 
  target_link_libraries (ele_test "-L${Thrift_LIB} -lthrift")

  ######MAIN ENTRY######
  find_package (Threads)
  add_executable(ele src/main.cpp)
  target_link_libraries(ele ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(ele -lrt)

endif()





