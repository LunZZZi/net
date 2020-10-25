add_executable(shut_client shut_client.cpp)
target_link_libraries(shut_client net)

add_executable(shut_server shut_server.cpp)
target_link_libraries(shut_server net)
