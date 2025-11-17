cmake_minimum_required(VERSION 3.10)

project(TCP_Client)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Client executable
add_executable(client client.cpp)

# No additional libraries needed for basic sockets on Linux
