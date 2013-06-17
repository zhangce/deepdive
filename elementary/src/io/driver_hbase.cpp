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


#include "driver_hbase.h"

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

void Driver_HBASE::add_frame(const int frameid)
{
  std::vector<apache::hadoop::hbase::thrift::Mutation> mutations;
  mutations.push_back(apache::hadoop::hbase::thrift::Mutation());
  mutations.back().column = "key:key";
             
  std::string bb((char*)empty_frame.content, framesize_in_byte);
  mutations.back().value = bb;
         
  std::string kk((char*)&frameid, sizeof(int));
        
  pthread_mutex_lock(mutex);
  client->mutateRow(tablename, kk, mutations, empty_attrs);
  pthread_mutex_unlock(mutex);
}

void Driver_HBASE::set_frame(const int frameid, const Frame& frame)
{
  std::vector<apache::hadoop::hbase::thrift::Mutation> mutations;
  mutations.push_back(apache::hadoop::hbase::thrift::Mutation());
  mutations.back().column = "key:key";
             
  std::string bb((char*)frame.content, framesize_in_byte);
  mutations.back().value = bb;
         
  std::string kk((char*)&frameid, sizeof(int));
        
  pthread_mutex_lock(mutex);
  client->mutateRow(tablename, kk, mutations, empty_attrs);
  pthread_mutex_unlock(mutex);
}

void Driver_HBASE::get_frame(const int frameid, Frame& frame)
{
  std::string kk((char*)&frameid, sizeof(int));
  pthread_mutex_lock(mutex);
  int scanner = client->scannerOpenWithStop(tablename, kk, kk, columnNames, empty_attrs);
  pthread_mutex_unlock(mutex);
  
  while (true) {
    std::vector<apache::hadoop::hbase::thrift::TRowResult> value;
    
    pthread_mutex_lock(mutex);
    client->scannerGet(value, scanner);
    pthread_mutex_unlock(mutex);
          
    if (value.size() == 0)
      break;
    for (size_t i = 0; i < value.size(); i++) {
      for (std::map<std::string, apache::hadoop::hbase::thrift::TCell>::const_iterator it = value[i].columns.begin(); it != value[i].columns.end(); ++it) {
	it->second.value.copy((char*) frame.content, framesize_in_byte);
        break;
      }
    }
    break;
  }
}

Driver_HBASE::Driver_HBASE(const int& _framesize_in_byte, std::string _conn_string): 
  Driver(_framesize_in_byte, _conn_string),
  empty_frame(Frame(_framesize_in_byte))
{
  std::vector<std::string> parsed;
  split(_conn_string, ':', parsed);
  assert(parsed.size()==3 && "Driver_HBASE: conn_string should be server:port:tablename");
  
  this->server = std::string(parsed[0].c_str());
  this->port = atoi(parsed[1].c_str());
  this->tablename = std::string(parsed[2].c_str());
  
  columnNames.push_back("key:");
  mutation.column = "key:key";
        
  mutex = new pthread_mutex_t;
  pthread_mutex_init(mutex, NULL);
        
  socket = boost::shared_ptr<apache::thrift::transport::TTransport>(new apache::thrift::transport::TSocket(this->server.c_str(), this->port));
  transport = boost::shared_ptr<apache::thrift::transport::TTransport>(new apache::thrift::transport::TBufferedTransport(socket));
  protocol = boost::shared_ptr<apache::thrift::protocol::TProtocol>(new apache::thrift::protocol::TBinaryProtocol(transport));
        
  client = new apache::hadoop::hbase::thrift::HbaseClient(protocol);
  transport->open();
        
  std::vector<apache::hadoop::hbase::thrift::ColumnDescriptor> columns;
  columns.push_back(apache::hadoop::hbase::thrift::ColumnDescriptor());
  columns.back().name = "key:";
  columns.back().maxVersions = 1;
        
  try {
    client->disableTable(this->tablename);
    client->deleteTable(this->tablename);
  }catch (const apache::hadoop::hbase::thrift::IOError &ioe){
    std::cerr << "WARN: " << ioe.message << std::endl;
    assert(false);
  }
        
  try{
    client->createTable(this->tablename, columns);
    client->enableTable(this->tablename);
  } catch (const apache::hadoop::hbase::thrift::AlreadyExists &ae) {
    std::cerr << "WARN: " << ae.message << std::endl;
    assert(false);
  }
  
}

Driver_HBASE::~Driver_HBASE()
{

}

