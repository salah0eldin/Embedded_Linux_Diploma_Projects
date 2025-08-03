#===================================================================================================
# Name        : CMakeLists.txt caesar_cipher
# Copyright   : Salah Eldin
# Description : A CMake file to compile and link the caesar_cipher module as a static library
#===================================================================================================

# ==================================================================================================
# Variables and Directories
# ==================================================================================================

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
set(CAESAR_CIPHER_LIB ${APP_LIB_DIR}/libcaesar_cipher.a)

# ==================================================================================================
# Targets and Rules
# ==================================================================================================

# Create object files in the specified directory (equivalent to gcc -c)
add_custom_command(
    OUTPUT ${APP_OBJ_DIR}/caesar_encrypt.o
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_OBJ_DIR}
    COMMAND gcc -c ${CAESAR_CIPHER_SRC_DIR}/caesar_encrypt.c -I ${CAESAR_CIPHER_INC_DIR} -o ${APP_OBJ_DIR}/caesar_encrypt.o
    DEPENDS ${CAESAR_CIPHER_SRC_DIR}/caesar_encrypt.c
    COMMENT "Building object file ${APP_OBJ_DIR}/caesar_encrypt.o from ${CAESAR_CIPHER_SRC_DIR}/caesar_encrypt.c..."
)

add_custom_command(
    OUTPUT ${APP_OBJ_DIR}/caesar_decrypt.o
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_OBJ_DIR}
    COMMAND gcc -c ${CAESAR_CIPHER_SRC_DIR}/caesar_decrypt.c -I ${CAESAR_CIPHER_INC_DIR} -o ${APP_OBJ_DIR}/caesar_decrypt.o
    DEPENDS ${CAESAR_CIPHER_SRC_DIR}/caesar_decrypt.c
    COMMENT "Building object file ${APP_OBJ_DIR}/caesar_decrypt.o from ${CAESAR_CIPHER_SRC_DIR}/caesar_decrypt.c..."
)

# Build the caesar_cipher static library from the object files (equivalent to ar -cr)
add_custom_command(
    OUTPUT ${CAESAR_CIPHER_LIB}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_LIB_DIR}
    COMMAND ar -cr ${CAESAR_CIPHER_LIB} ${APP_OBJ_DIR}/caesar_encrypt.o ${APP_OBJ_DIR}/caesar_decrypt.o
    DEPENDS ${APP_OBJ_DIR}/caesar_encrypt.o ${APP_OBJ_DIR}/caesar_decrypt.o
    COMMENT "Building caesar_cipher library..."
)

# Create a target that depends on the library
add_custom_target(caesar_cipher ALL DEPENDS ${CAESAR_CIPHER_LIB})

# Create necessary directories
add_custom_target(dirs_caesar
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_OBJ_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_LIB_DIR}
    COMMENT "Creating necessary directories..."
)

# Custom target for help message
add_custom_target(show_help_caesar
    COMMAND ${CMAKE_COMMAND} -E echo "Available targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  all       - Build the caesar_cipher library"
    COMMAND ${CMAKE_COMMAND} -E echo "  clean     - Remove generated object files and libraries"
    COMMAND ${CMAKE_COMMAND} -E echo "  dirs      - Create necessary directories"
    COMMAND ${CMAKE_COMMAND} -E echo "  show_help - Display this help message"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMENT "Displaying help message for caesar_cipher"
)
