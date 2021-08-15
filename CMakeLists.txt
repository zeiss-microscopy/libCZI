cmake_minimum_required(VERSION 3.0.2)

project(libCZI VERSION 0.45.0  )

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

include(TestLargeFile)
test_large_files (_LARGEFILES)

include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if (IS_BIG_ENDIAN)
 message("identified a big-endian platform")
else()
 message("identified a little-endian platform")
endif()

check_include_file_CXX(endian.h HAVE_ENDIAN_H)
if (NOT DEFINED HAVE_ENDIAN_H OR NOT HAVE_ENDIAN_H)
	set(HAVE_ENDIAN_H 0)
endif()
message("check for endian.h -> ${HAVE_ENDIAN_H}") 

include (CheckCXXSymbolExists)
check_cxx_symbol_exists(aligned_alloc stdlib.h HAVE_ALIGNED_ALLOC)
if (NOT HAVE_ALIGNED_ALLOC)
  check_cxx_symbol_exists(_aligned_malloc stdlib.h HAVE__ALIGNED_MALLOC)
endif()
 
include (CheckCXXSymbolExists)
check_cxx_symbol_exists(aligned_alloc stdlib.h HAVE_ALIGNED_ALLOC)
if (NOT HAVE_ALIGNED_ALLOC)
  check_cxx_symbol_exists(_aligned_malloc stdlib.h HAVE__ALIGNED_MALLOC)
endif()
 
# This option allows to exclude the unit-tests from the build. The unit-tests are using the
# Google-Test-framework which is downloaded from GitHub during the CMake-run.
option(LIBCZI_BUILD_UNITTESTS "Build the gTest-based unit-tests" ON) 

option(LIBCZI_BUILD_DYNLIB "Build dynamic library 'libCZI'." ON)
option(LIBCZI_BUILD_CZICMD "Build application 'CZICmd'." ON)

add_subdirectory(Src)