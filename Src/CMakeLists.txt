cmake_minimum_required(VERSION 3.1)

# This is a workaround for a problem in VisualStudio 2017 15.7. Without it, the CMake-generation in the IDE fails with a
# spurious exception -> https://github.com/Microsoft/VSLinux/issues/288
add_definitions(-DFOO)

#[[
CMake-information -> https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1
					 https://crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources/
]]

#[[
IF(UNIX)
# linking with 'thread' is necessary if we use std::thread and related under Linux it seems
#   otherwise - the program simply crashes (no build-error)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11 -fPIC -D_FILE_OFFSET_BITS=64")
   SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__ANSI__ -fPIC -D_FILE_OFFSET_BITS=64")
ENDIF(UNIX)
]]

enable_testing()

add_subdirectory(JxrDecode)
add_subdirectory(libCZI)
add_subdirectory(CZICmd)

add_subdirectory(libCZI_UnitTests)
