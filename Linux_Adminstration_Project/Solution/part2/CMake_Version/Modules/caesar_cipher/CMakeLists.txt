#===================================================================================================
# Name        : CMakeLists.txt caesar_cipher
# Copyright   : Salah Eldin
# Description : A CMake file to compile and link the caesar_cipher module as a static library
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
set(CAESAR_CIPHER_DIR ${MODULES_DIR}/caesar_cipher)
set(CAESAR_CIPHER_SRC_DIR ${CAESAR_CIPHER_DIR}/src)
set(CAESAR_CIPHER_INC_DIR ${CAESAR_CIPHER_DIR}/inc)

# Application Directories
set(APP_OUT_DIR ${APP_DIR}/out)
set(APP_OBJ_DIR ${APP_OUT_DIR}/gen)
set(APP_LIB_DIR ${APP_OUT_DIR}/libs)

# Modules Object files
set(CAESAR_CIPHER_OBJ ${APP_OBJ_DIR}/caesar_encrypt.o ${APP_OBJ_DIR}/caesar_decrypt.o)

# Modules Library files
set(CAESAR_CIPHER_LIB ${APP_LIB_DIR}/caesar_cipher.a)

# ==================================================================================================
# Targets and Rules
# ==================================================================================================

# Build the caesar_cipher static library
add_library(caesar_cipher STATIC
    ${CAESAR_CIPHER_SRC_DIR}/caesar_encrypt.c
    ${CAESAR_CIPHER_SRC_DIR}/caesar_decrypt.c
)

# Include directories for header files
target_include_directories(caesar_cipher PUBLIC
    ${CAESAR_CIPHER_INC_DIR}
)

# Set library output name and location
set_target_properties(caesar_cipher PROPERTIES
    OUTPUT_NAME "caesar_cipher"
    ARCHIVE_OUTPUT_DIRECTORY ${APP_LIB_DIR}
)

# Create necessary directories
add_custom_target(dirs_caesar
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_OBJ_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_LIB_DIR}
    COMMENT "Creating necessary directories..."
)

add_dependencies(caesar_cipher dirs_caesar)

# Custom target for help message
add_custom_target(show_help_caesar
    COMMAND ${CMAKE_COMMAND} -E echo "Available targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  all       - Build the caesar_cipher library"
    COMMAND ${CMAKE_COMMAND} -E echo "  clean     - Remove generated object files and libraries"
    COMMAND ${CMAKE_COMMAND} -E echo "  dirs      - Create necessary directories"
    COMMAND ${CMAKE_COMMAND} -E echo "  show_help - Display this help message"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Variables:"
    COMMAND ${CMAKE_COMMAND} -E echo "  -DVERBOSE=ON - Show all commands executed by make"
    COMMAND ${CMAKE_COMMAND} -E echo "                 (default: VERBOSE=OFF, commands are hidden)"
    COMMENT "Displaying help message for caesar_cipher"
)
