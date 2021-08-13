# This is a workaround for a problem in VisualStudio 2017 15.7. Without it, the CMake-generation in the IDE fails with a
# spurious exception -> https://github.com/Microsoft/VSLinux/issues/288
add_definitions(-DFOO)

include(FetchContent)

find_package(Eigen3 NO_MODULE)

file(GLOB LIBCZISRCFILES "*.h" "*.cpp" "*.hpp")
if (NOT TARGET Eigen3::Eigen)
 file(GLOB_RECURSE LIBCZISRCEIGENFILES "eigen/*")
endif()

# prepare the configuration-file "libCZI_Config.h"
if (IS_BIG_ENDIAN)
 set(libCZI_ISBIGENDIANHOST 1)
else()
 set(libCZI_ISBIGENDIANHOST 0)
endif()

if (HAVE_ALIGNED_ALLOC)
 set(libCZI_HAVE_ALIGNED_ALLOC 1)
else()
 set(libCZI_HAVE_ALIGNED_ALLOC 0)
endif()
if (HAVE__ALIGNED_MALLOC)
 set(libCZI_HAVE__ALIGNED_MALLOC 1)
else()
 set(libCZI_HAVE__ALIGNED_MALLOC 0)
endif()

set(libCZI_HAVE_ENDIAN_H ${HAVE_ENDIAN_H}) 

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" system_processor_lower )
if ((system_processor_lower  STREQUAL "sparc64") OR  # We are to determine whether we can load an integer from an unaligned address.
    (system_processor_lower  STREQUAL "sparc32") OR  # There should be a better way to do so - maybe compiling a test-program.
		(system_processor_lower  STREQUAL "sparc"))
	set (libCZI_CrashOnUnalignedIntegers 1)
else()
	set (libCZI_CrashOnUnalignedIntegers 0)
endif()

string(CONCAT libCZI_CompilerIdentification ${CMAKE_CXX_COMPILER_ID} " " ${CMAKE_CXX_COMPILER_VERSION} )

# get the URL of the upstream repository
execute_process(
  COMMAND git remote get-url origin
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE libCZI_REPOSITORYREMOTEURL
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (NOT DEFINED libCZI_REPOSITORYREMOTEURL OR "${libCZI_REPOSITORYREMOTEURL}" STREQUAL "")
	set(libCZI_REPOSITORYREMOTEURL "unknown")
endif()

execute_process(
  COMMAND git name-rev --name-only HEAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE libCZI_REPOSITORYBRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (NOT DEFINED libCZI_REPOSITORYBRANCH OR "${libCZI_REPOSITORYBRANCH}" STREQUAL "")
	set(libCZI_REPOSITORYBRANCH "unknown")
endif()

execute_process(
  COMMAND  git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE libCZI_REPOSITORYHASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (NOT DEFINED libCZI_REPOSITORYHASH OR "${libCZI_REPOSITORYHASH}" STREQUAL "")
	set(libCZI_REPOSITORYHASH "unknown")
endif()

FetchContent_Declare(
  zstd
  GIT_REPOSITORY https://github.com/facebook/zstd.git
)

if(NOT zstd_POPULATED)
	message(STATUS "Fetching zstd...")
	set(ZSTD_BUILD_PROGRAMS  OFF CACHE BOOL "" FORCE)
        set(ZSTD_BUILD_SHARED    OFF CACHE BOOL "" FORCE)

	FetchContent_Populate(zstd)

	add_subdirectory(${zstd_SOURCE_DIR}/build/cmake ${zstd_BINARY_DIR})
endif()

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/libCZI_Config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/libCZI_Config.h"
	ESCAPE_QUOTES @ONLY)
	

#  Define headers for this library. PUBLIC headers are used for compiling the library, and will be added to consumers' build paths.
set(libCZIPublicHeaders "ImportExport.h" "libCZI.h" "libCZI_Compositor.h" "libCZI_DimCoordinate.h" "libCZI_exceptions.h"
						   "libCZI_Helpers.h" "libCZI_Metadata.h" "libCZI_Metadata2.h" "libCZI_Pixels.h" "libCZI_ReadWrite.h" 
						   "libCZI_Site.h" "libCZI_Utilities.h" "libCZI_Write.h" "priv_guiddef.h")

#						  
#define the shared libCZI - library
#
add_library(libCZI SHARED ${LIBCZISRCFILES} ${LIBCZISRCEIGENFILES}  $<TARGET_OBJECTS:JxrDecodeStatic>)
target_link_libraries(libCZI PRIVATE libzstd_static)
set_target_properties(libCZI PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES) # https://crascit.com/2015/03/28/enabling-cxx11-in-cmake/
SET_TARGET_PROPERTIES (libCZI PROPERTIES DEFINE_SYMBOL  "LIBCZI_EXPORTS" )
set_target_properties(libCZI PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)
# add the binary tree to the search path for include files so that we will find libCZI_Config.h
target_include_directories(libCZI PRIVATE  "${CMAKE_CURRENT_BINARY_DIR}")
set_target_properties(libCZI PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(libCZI PROPERTIES PUBLIC_HEADER "${libCZIPublicHeaders}")
target_include_directories(libCZI PRIVATE ${zstd_SOURCE_DIR}/lib)


#
#define the static libCZI - library
#
# Notes: -we use JxrDecode as an "object-library" in order have it "embedded" into libCZI.a
add_library(libCZIStatic STATIC ${LIBCZISRCFILES} ${LIBCZISRCEIGENFILES} $<TARGET_OBJECTS:JxrDecodeStatic>)
target_link_libraries(libCZIStatic PRIVATE libzstd_static)
set_target_properties(libCZIStatic PROPERTIES CXX_STANDARD 11)
target_compile_definitions(libCZIStatic PRIVATE _LIBCZISTATICLIB)
set_target_properties(libCZIStatic PROPERTIES VERSION ${PROJECT_VERSION})
# add the binary tree to the search path for include files so that we will find libCZI_Config.h
target_include_directories(libCZIStatic PRIVATE  "${CMAKE_CURRENT_BINARY_DIR}")
set_target_properties(libCZIStatic PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(libCZIStatic PROPERTIES PUBLIC_HEADER "${libCZIPublicHeaders}")
target_include_directories(libCZIStatic PRIVATE ${zstd_SOURCE_DIR}/lib)

IF(UNIX)
  target_compile_options(libCZI PRIVATE -D_FILE_OFFSET_BITS=64)
  target_compile_options(libCZIStatic PRIVATE -D_FILE_OFFSET_BITS=64)
ENDIF(UNIX)


# TODO: instead of using a DEFINE, we should simply set the correct include-directory (if no eigen3-package is available)
if (TARGET Eigen3::Eigen)	# cf. https://eigen.tuxfamily.org/dox/TopicCMakeGuide.html
	MESSAGE("Eigen3 package found, it will be used for compilation of libCZI.")
	MESSAGE(${EIGEN3_INCLUDE_DIR})
	target_compile_definitions(libCZIStatic PRIVATE _USE_EXTERNAL_EIGEN3)
	target_compile_definitions(libCZI PRIVATE _USE_EXTERNAL_EIGEN3)
	target_include_directories(libCZIStatic PRIVATE ${EIGEN3_INCLUDE_DIR})
	target_include_directories(libCZI PRIVATE ${EIGEN3_INCLUDE_DIR})
else()
	MESSAGE("No Eigen3 package found, will use the bundled eigen3-code.")
endif(TARGET Eigen3::Eigen)

#
# Notes: - the variables CMAKE_INSTALL_<...> have been defined in the module "GNUInstallDirs"
#install(FILES ${libCZIPublicHeaders} DESTINATION  ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
install(TARGETS libCZI
#		 EXPORT "libCZI"
		 ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
		 LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
		 RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
		 PUBLIC_HEADER DESTINATION  ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

install(TARGETS libCZIStatic
#		 EXPORT "libCZIStatic"
		 ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
		 LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
		 RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
		 PUBLIC_HEADER DESTINATION  ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

#install(EXPORT "libCZI" DESTINATION "share/cmake")
#install(EXPORT "libCZIStatic" DESTINATION "share/cmake")
#export(TARGETS ${PROJECT_NAME} FILE libCZIConfig.cmake)

#SET_TARGET_PROPERTIES (libCZIStatic PROPERTIES UNDEF_SYMBOL  "LIBCZI_EXPORTS" )
