cmake_minimum_required(VERSION 3.16)

project(ozobus LANGUAGES CXX)

find_package(gRPC CONFIG REQUIRED)

add_executable(ozobus
  src/ozobus.cc
)

target_include_directories(ozobus PRIVATE 3rdparty)

target_link_libraries(ozobus PRIVATE gRPC::grpc++)
