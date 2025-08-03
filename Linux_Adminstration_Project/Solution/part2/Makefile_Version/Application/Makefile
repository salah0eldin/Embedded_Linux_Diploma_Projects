#===================================================================================================
# Name        : Makefile Project
# Copyright   : Salah Eldin
# Description : A Makefile to compile and link a modular program with separate source files for
#===================================================================================================

# ==================================================================================================
# Variables and Directories
# ==================================================================================================
# Variable to control verbosity of the output
VERBOSE = 0
ifeq ($(VERBOSE),0)
	V = @
else
	V = 
endif

echo_colored = @echo -e "\033[0;34m$(1)\033[0m"

# Main Directories
ROOT_DIR 				= ./..
APP_DIR 				= $(ROOT_DIR)/Application
MODULES_DIR 			= $(ROOT_DIR)/Modules

# Modules Directories
CAESAR_CIPHER_DIR 		= $(MODULES_DIR)/caesar_cipher
XOR_CIPHER_DIR 			= $(MODULES_DIR)/xor_cipher
CAESAR_CIPHER_INC_DIR 	= $(CAESAR_CIPHER_DIR)/inc
XOR_CIPHER_INC_DIR 		= $(XOR_CIPHER_DIR)/inc

# Application Directories
APP_OUT_DIR 			= $(APP_DIR)/out
APP_OBJ_DIR 			= $(APP_OUT_DIR)/gen
APP_LIB_DIR 			= $(APP_OUT_DIR)/libs
DIRS_LIST 				= $(APP_OBJ_DIR) $(APP_LIB_DIR)
APP_BIN 				= $(APP_DIR)/output

# Modules Library files
CAESAR_CIPHER_LIB 		= $(APP_LIB_DIR)/caesar_cipher.a
XOR_CIPHER_LIB 			= $(APP_LIB_DIR)/xor_cipher.so

# Modules makefiles
CAESAR_CIPHER_MK 		= caesar_cipher.mk
XOR_CIPHER_MK 			= xor_cipher.mk

# Application Source files
APP_SRC 				= $(APP_DIR)/app.c

# Application Object files
APP_OBJ 				= $(APP_OBJ_DIR)/app.o

# ==================================================================================================
# Targets and Rules
# ==================================================================================================
.PHONY: all clean help dirs

all: dirs $(APP_BIN)
	$(call echo_colored, "Build complete. Output binary is located at $(APP_BIN)")

# Build the application binary and link with the libraries
$(APP_BIN): $(APP_OBJ) $(CAESAR_CIPHER_LIB) $(XOR_CIPHER_LIB)
	$(call echo_colored, "Building output...")
	$(V)gcc $(APP_OBJ) -L$(APP_LIB_DIR) $(CAESAR_CIPHER_LIB) $(XOR_CIPHER_LIB) -o $(APP_BIN) -Wl,-rpath=$(APP_LIB_DIR)


# Build application object file
$(APP_OBJ): $(APP_SRC)
	$(call echo_colored, "Building application object...")
	$(V)gcc -c $< -I $(CAESAR_CIPHER_INC_DIR) -I $(XOR_CIPHER_INC_DIR) -o $@
	@echo

# Build the caesar_cipher
$(CAESAR_CIPHER_LIB):
	$(call echo_colored, "Building caesar_cipher library...")
	$(V)make $(if $(filter 1,$(VERBOSE)),VERBOSE=1,) -C $(CAESAR_CIPHER_DIR) -f $(CAESAR_CIPHER_MK)
	@echo

# Build the xor_cipher
$(XOR_CIPHER_LIB):
	$(call echo_colored, "Building xor_cipher library...")
	$(V)make $(if $(filter 1,$(VERBOSE)),VERBOSE=1,) -C $(XOR_CIPHER_DIR) -f $(XOR_CIPHER_MK)
	@echo

# Create necessary directories
dirs:
	@for dir in $(DIRS_LIST); do \
		if [ ! -d "$$dir" ]; then \
			echo -e "\033[0;34m Creating directory $$dir...\033[0m"; \
			mkdir -p "$$dir"; \
			echo; \
		fi \
	done

# Clean up generated files
clean:
	$(call echo_colored, "Cleaning up generated files...")
	$(V)rm -rf $(APP_BIN) $(APP_OBJ_DIR)/*.o $(APP_OUT_DIR)

# Help message
help:
	$(call echo_colored, "Available targets:")
	$(call echo_colored, "  all       - Build the application")
	$(call echo_colored, "  clean     - Remove generated files")
	$(call echo_colored, "  help      - Show this help message")
	$(call echo_colored, "")
	$(call echo_colored, "Variables:")
	$(call echo_colored, "  VERBOSE=1 - Show all commands executed by make")
	$(call echo_colored, "              (default: VERBOSE=0, commands are hidden)")
