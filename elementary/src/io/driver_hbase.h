/*
    Copyright 2013 <copyright holder> <email>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#ifndef DRIVER_HBASE_H
#define DRIVER_HBASE_H

#include <io/driver.h>
#include <io/hbase/Hbase.h>

#include <arpa/inet.h>

#include <time.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>

#include <iostream>

#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>

#include <io/hbase/Hbase.h>

class Driver_HBASE : public Driver
{

private:
  
  Frame empty_frame;
  
  pthread_mutex_t* mutex;
      
  boost::shared_ptr<apache::thrift::transport::TTransport> socket;
  boost::shared_ptr<apache::thrift::transport::TTransport> transport;
  boost::shared_ptr<apache::thrift::protocol::TProtocol> protocol;
  apache::hadoop::hbase::thrift::HbaseClient * client;
  
  std::map<apache::hadoop::hbase::thrift::Text, apache::hadoop::hbase::thrift::Text> empty_attrs;
    
  std::vector<std::string> columnNames;
  
  apache::hadoop::hbase::thrift::Mutation mutation;
  
  std::string server;
  int port;
  std::string tablename;

public:
  virtual void add_frame(const int frameid);
  virtual void set_frame(const int frameid, const Frame& frame);
  virtual void get_frame(const int frameid, Frame& frame);
  
  /**
   * _conn_string := server:port:tablename
   */
  Driver_HBASE(const int & _framesize_in_byte, std::string _conn_string);
  virtual ~Driver_HBASE();
};

#endif // DRIVER_HBASE_H
