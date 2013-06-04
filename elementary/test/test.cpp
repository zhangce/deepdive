
#include "gtest/gtest.h"
#include "test.h"

#include "io/driver_mm.h"
#include "io/driver_file.h"

int main(int argc, char **argv){
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
