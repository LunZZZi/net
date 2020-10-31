set(CMAKE_BUILD_TYPE Debug)
add_executable(nonblock_server nonblock_server.cpp)
target_link_libraries(nonblock_server net)
