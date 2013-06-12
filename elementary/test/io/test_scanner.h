#include "gtest/gtest.h"

#include "io/driver_file.h"
#include "io/scanner.h"


TEST (SCANNER_TEST, SCAN_DOUBLE){
  
  Scanner scanner(4096*1024, "/tmp/tmpscanner");
  double sum = 0;
  for(int i=0;i<10000000;i++){
    double value = 3.14*i;
    sum += value;
    scanner.push_record(&value, sizeof(double));
  }
  double rs = scanner.scan();
  EXPECT_EQ(rs, sum);
}