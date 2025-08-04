# Part 3: Process Management & Inter-Process Communication

Advanced process management demonstration using system calls and IPC mechanisms for embedded Linux system programming.

## Overview

This section implements a multi-process system monitoring application that demonstrates process creation, inter-process communication, signal handling, and system call integration - essential skills for embedded Linux development.

## Project Architecture

### Multi-Process Design:
- **Parent Process**: Orchestrates child processes and handles formatted output
- **Worker1 Process**: Executes `mpstat` command for CPU statistics
- **Worker2 Process**: Executes `ps` command for process information

### Key Technologies:
- Process management with `fork()` and `execv()`
- Inter-process communication using unnamed pipes
- Signal handling for graceful shutdown
- File descriptor manipulation and redirection

## File Structure

```
part3/
├── README.md          # This documentation
├── app.c              # Multi-process system monitor
├── a.out              # Compiled executable
└── record.mkv         # Demo recording (if available)
```

## Application Design

### `app.c` - Multi-Process System Monitor

**Core Components:**

1. **Process Creation & Management**
2. **Inter-Process Communication**
3. **Signal Handling**
4. **System Integration**

## Implementation Details

### 1. Process Creation & Management

```c
// Global variables for process coordination
int pipefd1[2], pipefd2[2];  // Communication pipes
int pid1, pid2;              // Process IDs
char buffer[1024];           // Data buffer
```

**Process Flow:**
```
Parent Process
├── fork() → Worker1 (mpstat)
├── fork() → Worker2 (ps)
└── Coordinate output collection
```

### 2. Inter-Process Communication (IPC)

**Pipe Setup:**
- `pipefd1[]`: Communication between Parent and Worker1
- `pipefd2[]`: Communication between Parent and Worker2

**File Descriptor Management:**
```c
// Worker processes redirect stdout to pipes
dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipe write-end
close(pipefd[0]);                // Close unused read-end
```

### 3. Signal Handling

**SIGINT Handler:**
```c
void int_handle(int signum) {
    wait(NULL);  // Wait for first child
    wait(NULL);  // Wait for second child  
    exit(0);     // Clean exit
}
```

**Features:**
- Graceful shutdown on Ctrl+C
- Prevents zombie processes
- Ensures proper resource cleanup

### 4. System Integration

**Worker1 - CPU Statistics:**
```c
char *args[] = {"mpstat", NULL};
execv("/usr/bin/mpstat", args);
```

**Worker2 - Process Information:**
```c
char *args[] = {"ps", NULL};
execv("/usr/bin/ps", args);
```

## Build and Run

### Compilation:
```bash
cd part3/
gcc app.c -o a.out
```

### Execution:
```bash
./a.out
```

### Expected Output:
```
Parent process PID: 1234
        Worker1 process started with PID: 1235
        Worker2 process started with PID: 1236

=== OUTPUT FROM WORKER1 (mpstat) ===
Linux 5.4.0-74-generic (hostname) 	08/04/25 	_x86_64_	(4 CPU)

Average:     CPU    %usr   %nice    %sys %iowait    %irq   %soft  %steal  %guest  %gnice   %idle
Average:     all    2.15    0.00    1.23    0.45    0.00    0.12    0.00    0.00    0.00   96.05

=== OUTPUT FROM WORKER2 (ps) ===
  PID TTY          TIME CMD
 1234 pts/0    00:00:00 bash
 1235 pts/0    00:00:00 a.out
 1236 pts/0    00:00:00 mpstat
 1237 pts/0    00:00:00 ps

=== ALL PROCESSES COMPLETED ===
```

### Termination:
```bash
# Press Ctrl+C for graceful shutdown
^C
# Application exits cleanly
```

## Key System Calls Demonstrated

### Process Management:
- `fork()` - Create child processes
- `execv()` - Replace process image with new program
- `wait()` - Wait for child process completion
- `getpid()` - Get process ID

### Inter-Process Communication:
- `pipe()` - Create communication pipes
- `dup2()` - Duplicate file descriptors
- `read()` - Read data from pipes
- `close()` - Close file descriptors

### Signal Handling:
- `signal()` - Register signal handler
- Custom handler for SIGINT (Ctrl+C)

## Process Execution Flow

```
1. Parent creates two pipes (pipefd1, pipefd2)
2. Parent forks Worker1
   ├── Worker1 redirects stdout to pipefd1
   ├── Worker1 executes mpstat
   └── Parent continues
3. Parent forks Worker2  
   ├── Worker2 redirects stdout to pipefd2
   ├── Worker2 executes ps
   └── Parent continues
4. Parent reads from pipefd1 and displays Worker1 output
5. Parent reads from pipefd2 and displays Worker2 output
6. Signal handler manages graceful shutdown
```

## Learning Outcomes

### Process Management
- **Process lifecycle** understanding and control
- **Parent-child relationships** and coordination
- **Process ID tracking** and management
- **Process synchronization** techniques

### Inter-Process Communication
- **Unnamed pipes** for data transfer
- **File descriptor manipulation** with dup2()
- **Synchronized data flow** control
- **Buffer management** for IPC data

### Signal Handling
- **Signal registration** and custom handlers
- **Graceful shutdown** procedures
- **Process cleanup** and resource management
- **Signal-safe programming** practices

### System Integration
- **System utility integration** (mpstat, ps)
- **Command execution** with execv()
- **Output formatting** and presentation
- **Error handling** and recovery

## Advanced Concepts

### File Descriptor Management:
- Understanding stdin, stdout, stderr (0, 1, 2)
- Redirection using dup2()
- Proper closure to prevent resource leaks

### Process States:
- Running, sleeping, zombie states
- Parent-child relationships
- Process cleanup and wait()

### IPC Mechanisms:
- Pipes vs. other IPC methods
- Synchronous vs. asynchronous communication
- Buffer management and flow control

## Common Use Cases in Embedded Linux

### System Monitoring:
- Real-time system statistics collection
- Process monitoring and management
- Resource usage tracking

### Multi-Process Applications:
- Sensor data collection with separate processes
- Real-time control systems
- Communication protocol handlers

### Signal Handling:
- Graceful shutdown in embedded systems
- Emergency stop procedures
- Inter-process notifications

## Prerequisites

- **Linux environment** with system utilities
- **GCC compiler** for compilation
- **System utilities**: `mpstat` and `ps` commands
- **Development tools**: Basic debugging knowledge

## Troubleshooting

### Common Issues:

**mpstat command not found:**
```bash
sudo apt-get install sysstat
```

**Permission denied:**
```bash
chmod +x a.out
```

**Zombie processes:**
- The signal handler should prevent this
- Check with `ps aux | grep defunct`

**Buffer overflow:**
- Current buffer size is 1024 bytes
- Increase if needed for larger outputs

## Security Considerations

- **Process privilege management**: Runs with user privileges
- **Resource limits**: Bounded by system limits
- **Input validation**: Commands are hardcoded for security
- **Signal handling**: Prevents resource leaks

---

**Note:** This implementation demonstrates fundamental concepts for embedded Linux process management. In production systems, consider additional error handling, logging, and resource management features.
