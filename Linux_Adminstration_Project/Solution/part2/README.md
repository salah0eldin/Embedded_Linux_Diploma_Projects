# Part 2: Build Systems Implementation

This section demonstrates proficiency with both modern (CMake) and traditional (Makefile) build systems for modular C applications in embedded Linux environments.

## Project Overview

A modular encryption/decryption application showcasing professional build system design and implementation. The project implements two cipher algorithms and demonstrates both static and shared library creation.

## Architecture

### Cipher Algorithms Implemented:
- **Caesar Cipher**: Classical substitution cipher with configurable key
- **XOR Cipher**: Simple but effective XOR-based encryption

### Application Features:
- **Command Line Interface**: `./output <caesar_key> <xor_key> <text>`
- **Dual Encryption**: Demonstrates both cipher methods sequentially
- **Real-time Processing**: Shows encryption and decryption steps
- **Modular Design**: Easy to extend with additional cipher algorithms

## Project Structure

```
part2/
├── README.md                          # This documentation
├── CMake.png                          # CMake build system diagram
├── Makefile.png                       # Makefile build system diagram
├── CMake_Version/                     # Modern build system implementation
│   ├── Application/
│   │   ├── app.c                      # Main application
│   │   ├── CMakeLists.txt             # Master build configuration
│   │   ├── output                     # Generated executable
│   │   └── build/                     # CMake build artifacts
│   └── Modules/
│       ├── caesar_cipher/             # Caesar cipher module
│       │   ├── CMakeLists.txt         # Module build config
│       │   ├── inc/caesar_cipher.h    # Header file
│       │   └── src/                   # Source files
│       │       ├── caesar_encrypt.c
│       │       └── caesar_decrypt.c
│       └── xor_cipher/                # XOR cipher module
│           ├── CMakeLists.txt         # Module build config
│           ├── inc/xor_cipher.h       # Header file
│           └── src/                   # Source files
│               ├── xor_encrypt.c
│               └── xor_decrypt.c
└── Makefile_Version/                  # Traditional build system
    ├── Application/
    │   ├── app.c                      # Main application (identical)
    │   ├── Makefile                   # Master Makefile
    │   └── output                     # Generated executable
    └── Modules/
        ├── caesar_cipher/
        │   ├── caesar_cipher.mk       # Module Makefile
        │   ├── inc/caesar_cipher.h
        │   └── src/                   # Source files
        └── xor_cipher/
            ├── xor_cipher.mk          # Module Makefile
            ├── inc/xor_cipher.h
            └── src/                   # Source files
```

## Build Systems Comparison

| Feature | CMake Version | Makefile Version |
|---------|---------------|------------------|
| **Build Tool** | Modern, cross-platform | Traditional, Unix-based |
| **Configuration** | Declarative CMakeLists.txt | Imperative Makefile |
| **Library Types** | Caesar (static .a), XOR (shared .so) | Caesar (static .a), XOR (shared .so) |
| **Dependency Management** | Automatic | Manual |
| **Out-of-source Build** | ✅ Supported | ❌ In-source |
| **Parallel Builds** | ✅ Automatic | ✅ With -j flag |
| **IDE Integration** | ✅ Excellent | ⚠️ Limited |
| **Colored Output** | ❌ Basic | ✅ Custom implementation |
| **Verbose Control** | ✅ Built-in | ✅ Custom VERBOSE variable |

## CMake Version

### Features Demonstrated:
- **Multi-directory project structure** with subdirectories
- **Static library creation** (Caesar cipher as .a file)
- **Shared library creation** (XOR cipher as .so file) 
- **Custom build targets and commands** for fine-grained control
- **Directory management automation** with organized output structure
- **Cross-module dependency handling** ensuring proper build order
- **Build artifact organization** in structured output directories

### Build Instructions:
```bash
cd CMake_Version/Application
mkdir build && cd build
cmake ..
make
./output 3 X "Hello World"
```

### CMake Targets Available:
```bash
make all          # Build everything
make clean        # Remove generated files
make show_help    # Display help message
make dirs         # Create necessary directories
```

### Key CMake Features Used:
- `add_subdirectory()` for modular builds
- `add_custom_command()` for custom build steps
- `add_custom_target()` for utility targets
- Custom output directories with `CMAKE_*_OUTPUT_DIRECTORY`
- Dependency management with `add_dependencies()`

## Makefile Version

### Advanced Makefile Features:
- **Colored output** for better user experience with custom echo functions
- **Verbose mode control** (`VERBOSE=0/1`) to show/hide commands
- **Automatic directory creation** with intelligent detection
- **Modular build** with sub-makefiles for each module
- **Comprehensive help system** with detailed usage information
- **Clean build artifact management** with organized cleanup

### Build Instructions:
```bash
cd Makefile_Version/Application
make
# or with verbose output:
make VERBOSE=1
./output 3 X "Hello World"
```

### Makefile Targets Available:
```bash
make all          # Build the application (default)
make clean        # Remove generated files
make help         # Show detailed help message
make dirs         # Create necessary directories
```

### Advanced Makefile Techniques:
- **Conditional verbosity** with variable-controlled command hiding
- **Colored output functions** using ANSI escape codes
- **Automatic dependency detection** for header file changes
- **Modular sub-makefiles** with parameter passing
- **Runtime library path** configuration with -rpath

## Module Details

### Caesar Cipher Module
```c
// API
void caesar_encrypt(char *text, int key);
void caesar_decrypt(char *text, int key);

// Implementation
- Alphabet-preserving encryption/decryption
- Handles both uppercase and lowercase letters
- Non-alphabetic characters remain unchanged
- Built as static library (.a)
```

### XOR Cipher Module
```c
// API  
void xor_encrypt(char *text, char key);
void xor_decrypt(char *text, char key);

// Implementation
- Simple XOR operation with single-character key
- Encryption and decryption use same operation
- Works on all ASCII characters
- Built as shared library (.so)
```

## Usage Examples

### Basic Usage:
```bash
./output 3 X "Hello World"
```

**Output:**
```
text encrypted with caesar: Khoor Zruog
text decrypted with caesar: Hello World
text encrypted with xor: �%__W�aW�_T
text decrypted with xor: Hello World
```

### Testing Different Keys:
```bash
# Caesar shift of 13 (ROT13) with XOR key 'A'
./output 13 A "Programming"

# Large Caesar shift with special XOR key
./output 25 @ "Embedded Linux"
```

## Build Output Structure

Both build systems create organized output directories:

```
Application/
├── output                          # Final executable
└── out/                            # Build artifacts
    ├── gen/                        # Object files (.o)
    │   ├── app.o
    │   ├── caesar_encrypt.o
    │   ├── caesar_decrypt.o
    │   ├── xor_encrypt.o
    │   └── xor_decrypt.o
    └── libs/                       # Libraries
        ├── libcaesar_cipher.a      # Static library
        └── libxor_cipher.so        # Shared library
```

## Learning Outcomes

### Build System Design
- Modern vs. traditional build system approaches
- Static vs. shared library creation and linking
- Dependency management and build order optimization
- Cross-platform build considerations

### C Programming Best Practices
- Modular code organization with clean interfaces
- Header/source file separation
- Library API design
- Memory-safe string manipulation

### Development Workflow
- Out-of-source vs. in-source builds
- Build artifact organization
- Incremental compilation optimization
- Professional development practices

## Prerequisites

- **GCC compiler** (gcc)
- **CMake** (version 3.0.0+) for CMake version
- **Make utility** for both versions
- **Linux environment** (tested on Ubuntu/Debian)

## Troubleshooting

### Common Issues:

**CMake not found:**
```bash
sudo apt-get install cmake
```

**Library not found at runtime:**
```bash
# Set LD_LIBRARY_PATH or use -rpath (already configured)
export LD_LIBRARY_PATH=$PWD/out/libs:$LD_LIBRARY_PATH
```

**Permission denied:**
```bash
chmod +x output
```

---

**Performance Note:** Both build systems are optimized for development workflow with fast incremental builds and clear error reporting.
