cmake_minimum_required(VERSION 2.8)

project(libCZI)

IF(UNIX)
# linking with 'thread' is necessary if we use std::thread and related under Linux it seems
#   otherwise - the program simply crashes (no build-error)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11 -fPIC -D_FILE_OFFSET_BITS=64")
   SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__ANSI__ -fPIC -D_FILE_OFFSET_BITS=64")
ENDIF(UNIX)

add_subdirectory(JxrDecode)
add_subdirectory(libCZI)

add_subdirectory(CZICmd)
