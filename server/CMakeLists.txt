cmake_minimum_required (VERSION 3.8)

add_executable (server server.cpp main.cpp executable_object.cpp interpreter.cpp)
include_directories(server, ../)

