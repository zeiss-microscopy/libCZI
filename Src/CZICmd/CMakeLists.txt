IF(UNIX)
 find_package(ZLIB QUIET)
 if (NOT ZLIB_FOUND)
    message(FATAL_ERROR [=[
    ZLIB library was not found, which is required for building. Consider installing
    like 'sudo apt-get install zlib1g-dev'.
    ]=])
 endif()

 find_package(PNG QUIET)
 if (NOT PNG_FOUND)
    message(FATAL_ERROR [=[
    PNG library was not found, which is required for building. Consider installing
    like 'sudo apt-get install libpng-dev'.
    ]=])
 endif()

ENDIF(UNIX)

file(GLOB CZICMDSRCFILES "*.h" "*.cpp" "*.hpp")
file(GLOB_RECURSE CZICMDRAPIDJSONSRCFILES "*.h" "*.cpp" "*.hpp")

add_executable(CZIcmd ${CZICMDSRCFILES} ${CZICMDRAPIDJSONSRCFILES})

set_target_properties(CZIcmd PROPERTIES CXX_STANDARD 11)
target_compile_definitions(CZIcmd PRIVATE _LIBCZISTATICLIB)

target_link_libraries(CZIcmd PRIVATE libCZIStatic ${ZLIB_LIBRARIES}  ${PNG_LIBRARIES} )
target_include_directories(CZIcmd PRIVATE ${ZLIB_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

IF(UNIX)
  # seems to be problem with glibc I'd reckon -> https://stackoverflow.com/questions/51584960/stdcall-once-throws-stdsystem-error-unknown-error-1
  target_link_libraries(CZIcmd  PUBLIC pthread)
ENDIF(UNIX)

set(CZICMD_USE_FREETYPE 0)
set(CZICMD_USE_WIC 0)
set(CZICMD_USE_GDIPLUS 0)
set(CZICMD_USE_LIBPNG 0)

IF (FREETYPE_FOUND)
	set(CZICMD_USE_FREETYPE 1)
	set(CZICMD_FREETYPE_VERSION_STRING "${FREETYPE_VERSION_STRING}")
ENDIF()

IF(PNG_FOUND)
	set(CZICMD_USE_LIBPNG 1)
	set(CZICMD_LIBPNG_VERSION_STRING "${PNG_VERSION_STRING}")
ENDIF()

IF(WIN32)
	set(CZICMD_USE_WIC 1)
	set(CZICMD_USE_GDIPLUS 1)
ENDIF()

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/CZIcmd_Config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/CZIcmd_Config.h"
  )


# checking platform -> https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/Checking-Platform
#                   -> https://stackoverflow.com/questions/9160335/os-specific-instructions-in-cmake-how-to

install(TARGETS CZIcmd RUNTIME  DESTINATION bin)
