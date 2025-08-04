# Part 1: User Management & Shell Scripting

This section focuses on Linux user management and shell scripting fundamentals essential for embedded Linux system administration.

## Overview

Advanced shell scripting project demonstrating user management, file operations, and system administration tasks commonly required in embedded Linux environments.

## Scripts Overview

### 1. `userdef` - Advanced User Creation Script

**Purpose:** Creates new users with custom specifications and configurations.

**Requirements:** 
- Must be run with sudo privileges
- Requires exactly 3 arguments

**Usage:**
```bash
sudo ./userdef <username> <password> <groupname>
```

**Features:**
- ✅ Argument validation (requires exactly 3 parameters)
- ✅ Root privilege verification
- ✅ Creates user with home directory and bash shell
- ✅ Sets user password programmatically (no prompts)
- ✅ Creates custom group with ID 200
- ✅ Assigns custom user ID (1600) and group membership
- ✅ Displays user/group information at each step
- ✅ Comprehensive error handling

**Example:**
```bash
sudo ./userdef edge1 password123 groupedge
```

### 2. `user_init` - Directory Setup Script

**Purpose:** Creates and populates user directories with development files.

**Features:**
- 🔍 Interactive username input with validation
- 🗑️ Automatic cleanup of existing directories
- 📁 Creates C development files (main.c, main.h, hello.c, hello.h)
- 📦 Tar compression and deployment to user home directory
- 🧹 Automatic cleanup of temporary files
- ✍️ Writes descriptive content to each file

**Usage:**
```bash
./user_init
# Follow the interactive prompts
```

**Process Flow:**
1. Prompts for username and validates existence
2. Removes existing `usr_dir` if present
3. Creates new directory with 4 C files
4. Writes descriptive content to each file
5. Compresses directory to tar.gz
6. Copies and extracts to user's home directory
7. Cleans up temporary files

### 3. `part1_q2_ghijkl.txt` - Advanced User Operations Script

**Purpose:** Interactive script for secure user directory management and operations.

**Features:**
- 🔐 Secure password input (hidden characters)
- ✅ User existence validation
- 📋 Directory listing with detailed permissions (`ls -r -l`)
- 👤 Smart ownership management with sudo privilege detection
- 🔍 File filtering and search operations
- 🗑️ Selective file cleanup (removes *.c files)
- 🛠️ Handles both sudoers and non-sudoers users with workarounds

**Usage:**
```bash
./part1_q2_ghijkl.txt
# Follow interactive prompts for username and password
```

**Smart Ownership Handling:**
- Detects if user has sudo privileges
- For sudoers: Uses `sudo chown` for proper ownership
- For non-sudoers: Uses copy/move workaround to maintain access

### 4. `undo` - Cleanup Script

**Purpose:** Removes test users and groups created during development.

**Usage:**
```bash
./undo
```

**Actions:**
- Removes user `edge1` and home directory (`-r` flag)
- Removes group `groupedge`
- Ensures clean system state for retesting

## File Structure

```
part1/
├── README.md                    # This documentation
├── userdef                      # Main user creation script
├── user_init                    # Directory setup script  
├── part1_q1_mno.txt             # Documentation for script deployment methods
├── part1_q2_ghijkl.txt          # User operations script
├── undo                         # Cleanup script
├── screen1_and_screen2.png      # Screenshot documentation
└── ...
```

## Learning Outcomes

### Shell Scripting Best Practices
- Proper argument validation and error handling
- Secure password handling techniques
- Interactive script design
- Process control and automation

### Linux User Management
- User creation with custom specifications
- Group management and membership
- User ID and Group ID assignment
- Home directory and shell configuration

### File Operations & Permissions
- Directory creation and management
- File permission analysis and modification
- Ownership management (chown/chmod)
- Archive creation and extraction

### System Security
- Privilege escalation handling
- Password security (hidden input)
- User validation and verification
- Safe cleanup procedures

## Usage Examples

### Complete Workflow Example:

```bash
# 1. Create a new user
sudo ./userdef testuser mypassword testgroup

# 2. Set up development directory for the user
./user_init
# Enter: testuser

# 3. Perform operations on user directory
./part1_q2_ghijkl.txt
# Enter: testuser
# Enter: mypassword

# 4. Clean up when done
./undo
```

### Script Deployment Methods (from part1_q1_mno.txt):

**Method 1: Add Script Directory to $PATH**
```bash
export PATH=$PATH:$(pwd)
```

**Method 2: Move Script to a bin Directory (Permanent)**
```bash
cp ./userdef $HOME/.local/bin/
```

**To make Method 1 permanent:**
Add the export line to `.bashrc` or `.profile` and source the file.

## Prerequisites

- Linux environment with bash shell
- sudo privileges for user management operations
- Standard Unix utilities (tar, grep, etc.)
- Write permissions in home directories

## Security Considerations

- Scripts validate user existence before operations
- Password input is hidden from terminal display
- Proper privilege checking before system modifications
- Safe handling of file operations with error checking
- Cleanup procedures to prevent system clutter

---

**Note:** These scripts are designed for educational purposes and development environments. Always review and test scripts thoroughly before use in production systems.
