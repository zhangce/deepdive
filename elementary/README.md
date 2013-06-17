##Elementary: Scalable Bayesian Inference and Learning

### Introduction

### Installation

Usually, Elementary is installed with [DeepDIVE](http://github.com/zhangce/deepdive),
which provides high-level interface (e.g., MLN, WinBUGS, Python, and SQL). However, Elementary
is able to be installed independently. When installed along, Elementary takes as input a
TSV-based low-level representation, and outputs inference and learning results also in a 
TSV-based format.

##### Requirements

We test Elementary on both Redhat and Ubuntu Linux. If only install Elementary without HBase and Accumulo 
support, the dependencies are pretty standard:

  - 64 bits Linux (tested with kernel > 2.6.32, though there should not be any problem with older kernels)
  - GCC (tested with 4.4.7 and 4.7.3)
  - googletest 1.6.0 (included with Elementary)

That says, to install Elementary, you probably only needs to install GCC on a 64bits Linux box.

If you want to use Elementary with HBase and Accumulo support, you need more dependencies:

  - [Boost](http://www.boost.org/)
  - [Thrift](http://thrift.apache.org/)

##### Installation without HBase and Accumulo Support

Installing Elementary with HBase and Accumulo requires a set of dependencies, e.g., Thrift and Boost.
However, to just play with Elementary, you can skip these dependencies by only using Unix files.
Using unix files can also scale to tera-bytes of data given you have enough space on your hard drive.

To install Elementary without HBase and Accumulo support, go into the elementary directory, and type in:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    
This should build two binaries:

  - **ele:** elementary binary
  - **ele_test:** elementary unit test cases

The simplest way to test your installation is to run unit tests by typing in

    $ make test            # (or ./ele_test)

The whole test might take about 10 minutes to finish. All test case should be passed. We expect 
outputs like

    ... ...
    
    [----------] 1 test from SCANNER_TEST
    [ RUN      ] SCANNER_TEST.SCAN_DOUBLE
      | Scaning 4 pages...[ETA=0.035021]...[TIME=0.0473778]...SIGSUM=[1.57e+12]
    [       OK ] SCANNER_TEST.SCAN_DOUBLE (107 ms)
    [----------] 1 test from SCANNER_TEST (107 ms total)
    
    [----------] 1 test from MODELACCESSOR_TEST
    [ RUN      ] MODELACCESSOR_TEST.LOAD
    [       OK ] MODELACCESSOR_TEST.LOAD (0 ms)
    [----------] 1 test from MODELACCESSOR_TEST (0 ms total)
    
    [----------] Global test environment tear-down
    [==========] 19 tests from 11 test cases ran. (237474 ms total)
    [  PASSED  ] 19 tests.

    

    
