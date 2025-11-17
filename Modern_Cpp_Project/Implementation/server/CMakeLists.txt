cmake_minimum_required(VERSION 3.10)

project(TCP_Server)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Server executable
add_executable(server server.cpp)

# No additional libraries needed for basic sockets on Linux
