set(CMAKE_BUILD_TYPE Debug)
add_executable(reactor_server reactor.cpp)
target_link_libraries(reactor_server net)
