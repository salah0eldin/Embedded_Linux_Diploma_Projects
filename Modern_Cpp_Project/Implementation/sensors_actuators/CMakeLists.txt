cmake_minimum_required(VERSION 3.16)

project(sensors_actuators VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Fetch dependencies
include(FetchContent)

# Fetch standalone Asio (header-only)
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

# Make available
FetchContent_MakeAvailable(asio)

set(PROJECT_SOURCES
        main.cpp
        sensors_actuators.cpp
        sensors_actuators.h
        sensors_actuators.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(sensors_actuators
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(sensors_actuators SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(sensors_actuators
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(sensors_actuators PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Link against pthread for threading support
find_package(Threads REQUIRED)
target_link_libraries(sensors_actuators PRIVATE Threads::Threads)

# Include Asio headers
target_include_directories(sensors_actuators PRIVATE ${asio_SOURCE_DIR}/asio/include)

# Define ASIO_STANDALONE for header-only mode
target_compile_definitions(sensors_actuators PRIVATE ASIO_STANDALONE)

# Include shared headers directory
target_include_directories(sensors_actuators PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Also keep source dir on include path for generated UI headers
target_include_directories(sensors_actuators PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.sensors_actuators)
endif()
set_target_properties(sensors_actuators PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS sensors_actuators
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(sensors_actuators)
endif()

# Custom target to run the sensors_actuators application
add_custom_target(run
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/sensors_actuators
    DEPENDS sensors_actuators
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running Sensors & Actuators application..."
    USES_TERMINAL
)
