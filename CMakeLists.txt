cmake_minimum_required(VERSION 3.6)
project(huffmanEncoder)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES huffmanEncoder.c list.h list.c decode.h decode.c)
add_executable(huffmanEncoder ${SOURCE_FILES})