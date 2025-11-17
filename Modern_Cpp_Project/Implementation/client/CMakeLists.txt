cmake_minimum_required(VERSION 3.10)

project(TCP_Client)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch dependencies
include(FetchContent)

# Fetch RapidYAML
FetchContent_Declare(
    ryml
    GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
    GIT_TAG v0.5.0
)

# Fetch standalone Asio (header-only)
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
)

FetchContent_MakeAvailable(ryml asio)

# Client executable
add_executable(client Client.cpp)

# Include Asio headers
target_include_directories(client PRIVATE ${asio_SOURCE_DIR}/asio/include)

# Define ASIO_STANDALONE for header-only mode
target_compile_definitions(client PRIVATE ASIO_STANDALONE)

# Link libraries
target_link_libraries(client PRIVATE ryml::ryml pthread)
