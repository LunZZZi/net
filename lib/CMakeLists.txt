set(net_lib
    read.cpp
)


add_library(net ${net_lib})
target_link_libraries(net pthread)

install(TARGETS net DESTINATION lib)

#file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/)