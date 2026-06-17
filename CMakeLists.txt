cmake_minimum_required(VERSION 3.16)

project(ozobus LANGUAGES CXX)

find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)

include(cmake/ProtoGen.cmake)
add_grpc_proto_library(ozobus_proto "${CMAKE_CURRENT_SOURCE_DIR}/protos")

add_executable(ozobus
  src/ozobus.cc
  src/ozobus/services/auth.cpp
)

target_include_directories(ozobus PRIVATE 3rdparty src)
target_link_libraries(ozobus PRIVATE gRPC::grpc++ ozobus_proto)
