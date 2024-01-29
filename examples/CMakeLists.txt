cmake_minimum_required(VERSION 3.8)

add_executable(tcp_client tcp_client.c)
target_link_libraries(tcp_client PUBLIC micro_sockets)

add_executable(tcp_server tcp_server.c)
target_link_libraries(tcp_server PUBLIC micro_sockets)
