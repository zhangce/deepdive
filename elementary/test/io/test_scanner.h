#include "gtest/gtest.h"

#include "io/driver_file.h"
#include "io/scanner.h"

double adddouble(char * buf, char * dummy, void * ppara){
  return *reinterpret_cast<double*>(buf);
}


TEST (SCANNER_TEST, SCAN_DOUBLE){
  
  Scanner<Driver_FILE> scanner(4096*1024, "/tmp/tmpscanner");
  double sum = 0;
  for(int i=0;i<1000000;i++){
    double value = 3.14*i;
    sum += value;
    scanner.push_record(&value, sizeof(double));
  }
  double rs = scanner.scan(&adddouble, NULL);
  EXPECT_EQ(rs, sum);
}