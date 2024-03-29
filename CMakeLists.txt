cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0091 NEW) # enable new "MSVC runtime library selection" (https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_RUNTIME_LIBRARY.html)
project(libCZI VERSION 0.45.0  )

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

# choose the appropriate MSVC-runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include(TestLargeFile)
test_large_files(_LARGEFILES)

include(check_unaligned_access)
CHECK_UNALIGNED_ACCESS(CRASH_ON_UNALIGNED_ACCESS)
if (CRASH_ON_UNALIGNED_ACCESS)
  message("Read/write to unaligned addresses is NOT possible.")
else()
  message("Read/write to unaligned addresses is OK.")
endif()

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

check_cxx_symbol_exists(aligned_alloc stdlib.h HAVE_ALIGNED_ALLOC)
if (NOT HAVE_ALIGNED_ALLOC)
  check_cxx_symbol_exists(_aligned_malloc stdlib.h HAVE__ALIGNED_MALLOC)
endif()

check_cxx_symbol_exists(open fcntl.h HAVE_FCNTL_H_OPEN)
check_cxx_symbol_exists(pread unistd.h HAVE_UNISTD_H_PREAD)
check_cxx_symbol_exists(pwrite unistd.h HAVE_UNISTD_H_PWRITE)
message("check for open -> ${HAVE_FCNTL_H_OPEN} ; check for pread -> ${HAVE_UNISTD_H_PREAD} ; check for pwrite -> ${HAVE_UNISTD_H_PWRITE}")


# This option allows to exclude the unit-tests from the build. The unit-tests are using the
# Google-Test-framework which is downloaded from GitHub during the CMake-run.
option(LIBCZI_BUILD_UNITTESTS "Build the gTest-based unit-tests" ON)

option(LIBCZI_BUILD_DYNLIB "Build dynamic library 'libCZI'." ON)
option(LIBCZI_BUILD_CZICMD "Build application 'CZICmd'." ON)

add_subdirectory(Src)