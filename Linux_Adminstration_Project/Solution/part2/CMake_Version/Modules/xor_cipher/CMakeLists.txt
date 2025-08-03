#===================================================================================================
# Name        : CMakeLists.txt xor_cipher
# Copyright   : Salah Eldin
# Description : A CMake file to compile and link the xor_cipher module as a shared library
#===================================================================================================

# ==================================================================================================
# Variables and Directories
# ==================================================================================================
# Variable to control verbosity of the output
option(VERBOSE "Show verbose output" OFF)
if(VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

# Main Directories
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(APP_DIR ${ROOT_DIR}/Application)
set(MODULES_DIR ${ROOT_DIR}/Modules)

# Modules Directories
set(XOR_CIPHER_DIR ${MODULES_DIR}/xor_cipher)
set(XOR_CIPHER_SRC_DIR ${XOR_CIPHER_DIR}/src)
set(XOR_CIPHER_INC_DIR ${XOR_CIPHER_DIR}/inc)

# Application Directories
set(APP_OUT_DIR ${APP_DIR}/out)
set(APP_OBJ_DIR ${APP_OUT_DIR}/gen)
set(APP_LIB_DIR ${APP_OUT_DIR}/libs)

# Modules Object files
set(XOR_CIPHER_OBJ ${APP_OBJ_DIR}/xor_encrypt.o ${APP_OBJ_DIR}/xor_decrypt.o)

# Modules Library files
set(XOR_CIPHER_LIB ${APP_LIB_DIR}/xor_cipher.so)

# ==================================================================================================
# Targets and Rules
# ==================================================================================================

# Build the xor_cipher shared library
add_library(xor_cipher SHARED
    ${XOR_CIPHER_SRC_DIR}/xor_encrypt.c
    ${XOR_CIPHER_SRC_DIR}/xor_decrypt.c
)

# Include directories for header files
target_include_directories(xor_cipher PUBLIC
    ${XOR_CIPHER_INC_DIR}
)

# Set library output name and location
set_target_properties(xor_cipher PROPERTIES
    OUTPUT_NAME "xor_cipher"
    LIBRARY_OUTPUT_DIRECTORY ${APP_LIB_DIR}
)

# Set position independent code for shared library (equivalent to -fPIC)
set_target_properties(xor_cipher PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Create necessary directories
add_custom_target(dirs_xor
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_OBJ_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_LIB_DIR}
    COMMENT "Creating necessary directories..."
)

add_dependencies(xor_cipher dirs_xor)

# Custom target for help message
add_custom_target(show_help_xor
    COMMAND ${CMAKE_COMMAND} -E echo "Available targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  all       - Build the xor_cipher library"
    COMMAND ${CMAKE_COMMAND} -E echo "  clean     - Remove generated object files and libraries"
    COMMAND ${CMAKE_COMMAND} -E echo "  dirs      - Create necessary directories"
    COMMAND ${CMAKE_COMMAND} -E echo "  show_help - Display this help message"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Variables:"
    COMMAND ${CMAKE_COMMAND} -E echo "  -DVERBOSE=ON - Show all commands executed by make"
    COMMAND ${CMAKE_COMMAND} -E echo "                 (default: VERBOSE=OFF, commands are hidden)"
    COMMENT "Displaying help message for xor_cipher"
)
