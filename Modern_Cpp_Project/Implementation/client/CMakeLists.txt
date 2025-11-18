cmake_minimum_required(VERSION 3.10)

project(Client)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Speed up compilation
set(CMAKE_BUILD_TYPE Release)  # Use Release mode for faster builds
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")  # Optimization level 2

# Fetch dependencies
include(FetchContent)

# Speed up FetchContent by using shallow clones
set(FETCHCONTENT_QUIET FALSE)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)  # Don't check for updates every build

# Fetch RapidYAML
FetchContent_Declare(
    ryml
    GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
    GIT_TAG v0.5.0
    GIT_SHALLOW TRUE  # Shallow clone (faster)
    GIT_PROGRESS TRUE
)

# Fetch standalone Asio (header-only)
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
    GIT_SHALLOW TRUE  # Shallow clone (faster)
    GIT_PROGRESS TRUE
)

# Make available with minimal rebuilds
FetchContent_MakeAvailable(ryml asio)

# Client executable
add_executable(client Client.cpp)

# Include Asio headers
target_include_directories(client PRIVATE ${asio_SOURCE_DIR}/asio/include)

# Include shared headers
target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Define ASIO_STANDALONE for header-only mode
target_compile_definitions(client PRIVATE ASIO_STANDALONE)

# Link libraries
target_link_libraries(client PRIVATE ryml::ryml)

# Link threading library (pthread on Linux/Unix, native on Windows)
find_package(Threads REQUIRED)
target_link_libraries(client PRIVATE Threads::Threads)

# Windows-specific: Link ws2_32 and mswsock for Asio sockets
if(WIN32)
    target_link_libraries(client PRIVATE ws2_32 mswsock)
endif()

# Custom target to run the client with config file
add_custom_target(run
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/client -c ${CMAKE_CURRENT_SOURCE_DIR}/../config.yaml
    DEPENDS client
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running client with config file..."
    USES_TERMINAL
)
