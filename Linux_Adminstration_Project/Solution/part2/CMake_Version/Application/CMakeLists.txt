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
# First create the application object file
add_custom_command(
    OUTPUT ${APP_OBJ_DIR}/app.o
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_OBJ_DIR}
    COMMAND gcc -c ${CMAKE_CURRENT_SOURCE_DIR}/${APP_SRC} -I ${CAESAR_CIPHER_INC_DIR} -I ${XOR_CIPHER_INC_DIR} -o ${APP_OBJ_DIR}/app.o
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${APP_SRC}
    COMMENT "Building application object..."
)

# Build the final executable from object file and libraries
add_custom_command(
    OUTPUT ${APP_BIN}
    COMMAND gcc ${APP_OBJ_DIR}/app.o -L${APP_LIB_DIR} ${CAESAR_CIPHER_LIB} ${XOR_CIPHER_LIB} -o ${APP_BIN} -Wl,-rpath=${APP_LIB_DIR}
    DEPENDS ${APP_OBJ_DIR}/app.o ${CAESAR_CIPHER_LIB} ${XOR_CIPHER_LIB}
    COMMENT "Building output..."
)

# Create a target that depends on the final executable
add_custom_target(output ALL DEPENDS ${APP_BIN})

# Make sure libraries are built before the executable
add_dependencies(output caesar_cipher xor_cipher)

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
    COMMENT "Displaying help message"
)

# Override the default clean target to also remove the out directory
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
    ${APP_OUT_DIR}
    ${APP_BIN}
)