cmake_minimum_required(VERSION 3.1)
project(net)

set(CMAKE_CXX_STANDARD 11)

# Put the libaries and binaries that get built into directories at the
# top of the build tree rather than in hard-to-find leaf directories.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(lib/)
add_subdirectory(echo/)
add_subdirectory(shut/)
add_subdirectory(pingpong/)
add_subdirectory(echo2/)
add_subdirectory(ls/)
add_subdirectory(poll/)
add_subdirectory(nonblock/)
add_subdirectory(reactor/)
