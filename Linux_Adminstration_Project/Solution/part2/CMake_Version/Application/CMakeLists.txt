#===================================================================================================
# Name        : CMakeLists.txt Project
# Copyright   : Salah Eldin
# Description : A CMake project to compile and link a modular program with separate source files for
#===================================================================================================

cmake_minimum_required(VERSION 3.0.0)

project(
    cipher_cmake 
    VERSION 1.0.0 
    DESCRIPTION "CMake project for cipher example"
    LANGUAGES C
)

# ==================================================================================================
# Variables and Directories
# ==================================================================================================
# Variable to control verbosity of the output
option(VERBOSE "Show verbose output" OFF)
if(VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

# Main Directories
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(APP_DIR ${ROOT_DIR}/Application)
set(MODULES_DIR ${ROOT_DIR}/Modules)

# Modules Directories
set(CAESAR_CIPHER_DIR ${MODULES_DIR}/caesar_cipher)
set(XOR_CIPHER_DIR ${MODULES_DIR}/xor_cipher)
set(CAESAR_CIPHER_INC_DIR ${CAESAR_CIPHER_DIR}/inc)
set(XOR_CIPHER_INC_DIR ${XOR_CIPHER_DIR}/inc)

# Application Directories
set(APP_OUT_DIR ${APP_DIR}/out)
set(APP_OBJ_DIR ${APP_OUT_DIR}/gen)
set(APP_LIB_DIR ${APP_OUT_DIR}/libs)
set(APP_BIN ${APP_DIR}/output)

# Modules Library files
set(CAESAR_CIPHER_LIB ${APP_LIB_DIR}/libcaesar_cipher.a)
set(XOR_CIPHER_LIB ${APP_LIB_DIR}/libxor_cipher.so)

# Application Source files
set(APP_SRC app.c)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${APP_LIB_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${APP_LIB_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${APP_DIR})

# Set object file output directory for all targets
set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

# ==================================================================================================
# Targets and Rules
# ==================================================================================================

# Add subdirectories for modules (equivalent to calling separate makefiles)
add_subdirectory(${CAESAR_CIPHER_DIR} caesar_cipher_build)
add_subdirectory(${XOR_CIPHER_DIR} xor_cipher_build)

# Build the application binary and link with the libraries
add_executable(output ${APP_SRC})

# Include directories for header files
target_include_directories(output PRIVATE
    ${CAESAR_CIPHER_INC_DIR}
    ${XOR_CIPHER_INC_DIR}
)

# Link with the libraries
target_link_libraries(output 
    caesar_cipher
    xor_cipher
)

# Set RPATH for shared libraries
set_target_properties(output PROPERTIES
    INSTALL_RPATH ${APP_LIB_DIR}
    BUILD_WITH_INSTALL_RPATH TRUE
)

# Custom target to create necessary directories
add_custom_target(dirs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_OBJ_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_LIB_DIR}
    COMMENT "Creating necessary directories..."
)

# Add dependencies
add_dependencies(caesar_cipher dirs)
add_dependencies(xor_cipher dirs)
add_dependencies(output dirs)

# Custom target for help message
add_custom_target(show_help
    COMMAND ${CMAKE_COMMAND} -E echo "Available targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  all       - Build the application"
    COMMAND ${CMAKE_COMMAND} -E echo "  clean     - Remove generated files"
    COMMAND ${CMAKE_COMMAND} -E echo "  show_help - Show this help message"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Variables:"
    COMMAND ${CMAKE_COMMAND} -E echo "  -DVERBOSE=ON - Show all commands executed by make"
    COMMAND ${CMAKE_COMMAND} -E echo "                 (default: VERBOSE=OFF, commands are hidden)"
    COMMENT "Displaying help message"
)