# downlad and build googletest -> https://github.com/google/googletest/tree/master/googletest , https://crascit.com/2015/07/25/cmake-gtest/

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" . RESULT_VARIABLE result  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/googletest-download" )

if(result)
	message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build . RESULT_VARIABLE result  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )

if(result)
	message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
					${CMAKE_CURRENT_BINARY_DIR}/googletest-build
					EXCLUDE_FROM_ALL)


file(GLOB SRCS *.cpp)
ADD_EXECUTABLE(libCZI_UnitTests ${SRCS})

# Now simply link against gtest or gtest_main as needed. Eg
TARGET_LINK_LIBRARIES(libCZI_UnitTests
	libCZIStatic
	gtest
	gmock
)

set_target_properties(libCZI_UnitTests PROPERTIES INCLUDE_DIRECTORIES "${gtest_SOURCE_DIR}/include")

set_target_properties(libCZI_UnitTests PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES) 

add_test(NAME libCZI_UnitTests COMMAND libCZI_UnitTests)

