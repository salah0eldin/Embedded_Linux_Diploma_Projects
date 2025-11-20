cmake_minimum_required(VERSION 3.16)

project(server VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

# Fetch dependencies
include(FetchContent)

# Fetch RapidYAML
FetchContent_Declare(
    ryml
    GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
    GIT_TAG v0.5.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

# Fetch standalone Asio (header-only)
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

# Make available
FetchContent_MakeAvailable(ryml asio)

set(PROJECT_SOURCES
        main.cpp
        server.cpp
        server.h
        server.ui
        resources.qrc
        ServerConfigParser.hpp
        ../include/TCPSocket.cpp
        ../include/UDPSocket.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(server
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET server APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(server SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(server
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(server PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)

# Link against pthread for threading support
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)

# Link RapidYAML
target_link_libraries(server PRIVATE ryml::ryml)

# Include Asio headers
target_include_directories(server PRIVATE ${asio_SOURCE_DIR}/asio/include)

# Define ASIO_STANDALONE for header-only mode
target_compile_definitions(server PRIVATE ASIO_STANDALONE)

# Include shared headers directory
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Also keep server source dir on include path for generated UI headers
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.server)
endif()
set_target_properties(server PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS server
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(server)
endif()

# Custom target to run the server with config file
add_custom_target(run
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/server ${CMAKE_CURRENT_SOURCE_DIR}/../config.yaml
    DEPENDS server
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running server with config file..."
    USES_TERMINAL
)
