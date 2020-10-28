add_executable(ls_client ls_client.cpp message.cpp)
target_link_libraries(ls_client net)

add_executable(ls_server ls_server.cpp message.cpp)
target_link_libraries(ls_server net)
