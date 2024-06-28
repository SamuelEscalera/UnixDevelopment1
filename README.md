# WEB SERVER

**Samuel Escalera Herrera**
## Project Description

This project consists of a service that runs using a NET socket with a maximum of 30 connections. The service receives the port as a program argument and each client connection is handled by a thread. The service also handles the SIGINT (Ctrl+C) signal for clean termination.

The service accepts the following commands:

- `getInfo`: Returns the service name and version. Example: "Mike's service, v0.1".
- `getNumberOfPartitions`: Returns the number of partitions on the system.
- `getCurrentKernelVersion`: Returns the system kernel version.
- `sshdRunning`: Returns `true` or `false` depending on whether the sshd service is running.

## Project files

- Main.c**: Server implementation.
- Client.c**: Implementation of a client to test the server.
- **CMakeLists.txt**: CMake configuration file to compile the project.

## Requirements

- CMake 3.10 or higher
- A compatible C compiler

## Compiling

To compile the project, follow these steps:

1. Clone the repository.
2. Navigate to the build folder
3. Run `cmake ..` to generate the build files.
4. Run `cmake --build .` to compile the project.

```bash
git clone https://github.com/SamuelEscalera/UnixDevelopment1.git
cd UnixDevelopment1
cd build
cmake .
cmake --build .