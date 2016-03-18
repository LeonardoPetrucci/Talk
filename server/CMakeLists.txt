cmake_minimum_required(VERSION 3.3)
project(TalkServer)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c9911")

set(SOURCE_FILES server.c chat.c threads.c)
set(CMAKE_USE_PTHREAD_INIT -lpthread)
add_executable(server ${SOURCE_FILE})

