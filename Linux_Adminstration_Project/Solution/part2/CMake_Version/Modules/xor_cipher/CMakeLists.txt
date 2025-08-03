#===================================================================================================
# Name        : CMakeLists.txt xor_cipher
# Copyright   : Salah Eldin
# Description : A CMake file to compile and link the xor_cipher module as a shared library
#===================================================================================================

# ==================================================================================================
# Variables and Directories
# ==================================================================================================

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
set(XOR_CIPHER_LIB ${APP_LIB_DIR}/libxor_cipher.so)

# ==================================================================================================
# Targets and Rules
# ==================================================================================================

# Create object files in the specified directory (equivalent to gcc -c -fPIC)
add_custom_command(
    OUTPUT ${APP_OBJ_DIR}/xor_encrypt.o
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_OBJ_DIR}
    COMMAND gcc -c ${XOR_CIPHER_SRC_DIR}/xor_encrypt.c -I ${XOR_CIPHER_INC_DIR} -o ${APP_OBJ_DIR}/xor_encrypt.o -fPIC
    DEPENDS ${XOR_CIPHER_SRC_DIR}/xor_encrypt.c
    COMMENT "Building object file ${APP_OBJ_DIR}/xor_encrypt.o from ${XOR_CIPHER_SRC_DIR}/xor_encrypt.c..."
)

add_custom_command(
    OUTPUT ${APP_OBJ_DIR}/xor_decrypt.o
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_OBJ_DIR}
    COMMAND gcc -c ${XOR_CIPHER_SRC_DIR}/xor_decrypt.c -I ${XOR_CIPHER_INC_DIR} -o ${APP_OBJ_DIR}/xor_decrypt.o -fPIC
    DEPENDS ${XOR_CIPHER_SRC_DIR}/xor_decrypt.c
    COMMENT "Building object file ${APP_OBJ_DIR}/xor_decrypt.o from ${XOR_CIPHER_SRC_DIR}/xor_decrypt.c..."
)

# Build the xor_cipher shared library from the object files (equivalent to gcc -shared)
add_custom_command(
    OUTPUT ${XOR_CIPHER_LIB}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_LIB_DIR}
    COMMAND gcc -shared -o ${XOR_CIPHER_LIB} ${APP_OBJ_DIR}/xor_encrypt.o ${APP_OBJ_DIR}/xor_decrypt.o
    DEPENDS ${APP_OBJ_DIR}/xor_encrypt.o ${APP_OBJ_DIR}/xor_decrypt.o
    COMMENT "Building xor_cipher library..."
)

# Create a target that depends on the library
add_custom_target(xor_cipher ALL DEPENDS ${XOR_CIPHER_LIB})

# Create necessary directories
add_custom_target(dirs_xor
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_OBJ_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_LIB_DIR}
    COMMENT "Creating necessary directories..."
)

# Custom target for help message
add_custom_target(show_help_xor
    COMMAND ${CMAKE_COMMAND} -E echo "Available targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  all       - Build the xor_cipher library"
    COMMAND ${CMAKE_COMMAND} -E echo "  clean     - Remove generated object files and libraries"
    COMMAND ${CMAKE_COMMAND} -E echo "  dirs      - Create necessary directories"
    COMMAND ${CMAKE_COMMAND} -E echo "  show_help - Display this help message"
    COMMAND ${CMAKE_COMMAND} -E echo ""
   COMMENT "Displaying help message for xor_cipher"
)
