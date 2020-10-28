add_executable(echo2_client echo2_client.cpp)
target_link_libraries(echo2_client net)

add_executable(echo2_server echo2_server.cpp)
target_link_libraries(echo2_server net)
