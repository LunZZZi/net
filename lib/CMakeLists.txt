set(net_lib
    read.cpp
    write.cpp
    acceptor.cpp
    buffer.cpp
    channel_map.cpp
    channel.cpp
    event_loop_thread.cpp
    event_loop.cpp
    log.cpp
    poll_dispatcher.cpp
    tcp_connection.cpp
    thread_pool.cpp
    tcp_server.cpp
    thread_pool.cpp
    utils.cpp

)


add_library(net ${net_lib})
target_link_libraries(net pthread)

install(TARGETS net DESTINATION lib)

#file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/)