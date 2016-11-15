cmake_minimum_required(VERSION 2.8)
project("fs_stub")

file(GLOB SRC "*.c" "*.h")
add_library(${PROJECT_NAME} SHARED ${SRC})
