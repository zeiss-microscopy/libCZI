cmake_minimum_required(VERSION 3.1)

# This is a workaround for a problem in VisualStudio 2017 15.7. Without it, the CMake-generation in the IDE fails with a
# spurious exception -> https://github.com/Microsoft/VSLinux/issues/288
add_definitions(-DFOO)


enable_testing()

add_subdirectory(JxrDecode)
add_subdirectory(libCZI)
add_subdirectory(CZICmd)
add_subdirectory(libCZI_UnitTests)
