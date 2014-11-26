#project(TCP)
#cmake_minimum_required(VERSION 2.8)
#aux_source _directory(. SRC_LIST)
#add_executable(${PROJECT_NAME} ${SRC_LIST})

project(TCP)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

add_library(tcp_server STATIC tcp_server.cpp)
add_library(tcp_client STATIC tcp_client.cpp)
add_library(tcp_socket STATIC tcp_socket.cpp)
add_library(tcp_exception STATIC tcp_exception.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(tcp_server_exec main_server.cpp)
add_executable(tcp_client_exec main_client.cpp)

target_link_libraries(tcp_server_exec tcp_server tcp_socket tcp_exception)
target_link_libraries(tcp_client_exec tcp_client)
