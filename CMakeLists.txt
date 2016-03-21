cmake_minimum_required(VERSION 3.3)
project(So)

set(SOURCE_FILE server.c chat.c commands.c msg_queue.c semaphore.c threads.c)

add_executable(server ${SOURCE_FILE})