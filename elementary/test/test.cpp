
#include "gtest/gtest.h"
#include "test.h"

/*
#include "io/test_frame.h"
#include "io/test_driver_file.h"
#include "io/test_scanner.h"
#include "io/test_buffer.h"
#include "io/test_fixlenkv.h"
#include "io/test_varlenkv.h"
#include "fg/test_factoraccessor.h"
#include "fg/test_variablescanner.h"
*/

#include "fg/test_factorgraph.h"


int main(int argc, char **argv){
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
