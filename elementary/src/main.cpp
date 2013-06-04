
#include <iostream>

#include "io/driver_mm.h"
#include "io/driver_file.h"

class page{
public:
  int content[128];
};

int main(int argc, char **argv){
  std::cout << "Hello World!" << std::endl;
  
  /*
  Driver_MM<int> mm;
  mm.init(0);
  
  mm.init(1);
  
  mm.set(12, NULL);
  */
  
  Driver_FILE<page> mm("/tmp/test.txt");
  mm.init(0);
  
  mm.init(1);
  
  page p;
  p.content[0]=12;
  
  mm.set(0, p);
  
  page p2;
  mm.get(0, p2);
  
  std::cout << p2.content[0] << std::endl;
  
  
  return 0;
}