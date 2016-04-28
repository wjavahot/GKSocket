cmake_minimum_required(VERSION 3.5)
project(gk_mobile_socket)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

set(SOURCE_FILES
    CMakeLists.txt
    gk_socket_kqueue.h
    gk_socket_poll.h
    gk_code_util.h
    gk_code_util.c
    gk_socket_common.h
    gk_socket_common.c
    gk_delim_framer.h
    gk_delim_framer.c
    gk_message.c
    gk_message.h
    gk_command.c
    gk_command.h
    gk_socket_manager.h
    gk_socket_manger.c
    gk_socket_main.c)

add_executable(gk_mobile_socket ${SOURCE_FILES})