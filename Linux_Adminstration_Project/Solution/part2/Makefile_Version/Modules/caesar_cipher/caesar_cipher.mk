#===================================================================================================
# Name        : caesar_cipher.mk
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
ROOT_DIR 				= ./../..
APP_DIR 				= $(ROOT_DIR)/Application
MODULES_DIR 			= $(ROOT_DIR)/Modules

# Modules Directories
CAESAR_CIPHER_DIR 		= $(MODULES_DIR)/caesar_cipher
CAESAR_CIPHER_SRC_DIR 	= $(CAESAR_CIPHER_DIR)/src
CAESAR_CIPHER_INC_DIR 	= $(CAESAR_CIPHER_DIR)/inc

# Application Directories
APP_OUT_DIR 			= $(APP_DIR)/out
APP_OBJ_DIR 			= $(APP_OUT_DIR)/gen
APP_LIB_DIR 			= $(APP_OUT_DIR)/libs

# Modules Object files
CAESAR_CIPHER_OBJ 		= $(APP_OBJ_DIR)/caesar_encrypt.o $(APP_OBJ_DIR)/caesar_decrypt.o

# Modules Library files
CAESAR_CIPHER_LIB 		= $(APP_LIB_DIR)/caesar_cipher.a

# ==================================================================================================
# Targets and Rules
# ==================================================================================================
.PHONY: all clean help dirs

all: dirs $(CAESAR_CIPHER_LIB)
	$(call echo_colored, "Build complete. Output library is located at $(CAESAR_CIPHER_LIB)")


$(CAESAR_CIPHER_LIB): $(CAESAR_CIPHER_OBJ)
	$(call echo_colored, "Building caesar_cipher library...")
	$(V)ar -cr $@ $^

$(APP_OBJ_DIR)/%.o: $(CAESAR_CIPHER_SRC_DIR)/%.c
	$(call echo_colored, "Building object file $@ from $<...")
	$(V)gcc -c $< -I $(CAESAR_CIPHER_INC_DIR) -o $@

# Create necessary directories
dirs:
	@for dir in $(DIRS_LIST); do \
		if [ ! -d "$$dir" ]; then \
			echo -e "\033[0;34m Creating directory $$dir...\033[0m"; \
			mkdir -p "$$dir"; \
			echo; \
		fi \
	done

clean:
	$(call echo_colored, "Cleaning up generated files...")
	$(V)rm $(CAESAR_CIPHER_OBJ) $(CAESAR_CIPHER_LIB)

help:
	$(call echo_colored, "Available targets:")
	$(call echo_colored, "  all       - Build the caesar_cipher library")
	$(call echo_colored, "  clean     - Remove generated object files and libraries")
	$(call echo_colored, "  dirs      - Create necessary directories")
	$(call echo_colored, "  help      - Display this help message")
	$(call echo_colored, "")
	$(call echo_colored, "Variables:")
	$(call echo_colored, "  VERBOSE=1 - Show all commands executed by make")
	$(call echo_colored, "              (default: VERBOSE=0, commands are hidden)")
