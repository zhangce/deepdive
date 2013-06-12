
#include "gtest/gtest.h"
#include "test.h"


//#include "io/test_frame.h"
//#include "io/test_driver_file.h"

#include "io/test_scanner.h"

int main(int argc, char **argv){
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
