# TCP/IP Remote Management Tool

## Overview
The TCP/IP Remote Management Tool is a command-line utility designed to manage processes on a remote machine over a TCP/IP connection. This tool enables system administrators to perform various operations such as retrieving process information, monitoring resource usage, managing ports, and controlling processes remotely.

## Features
- **Process Management**
  - List all running processes
  - Monitor memory usage by process
  - Track CPU utilization by process
  - View ports used by specific processes
  - Kill processes using signal **-15()**
  - Restart processes

## Requirements
- **Operating System**: Linux-based systems (tested on Ubuntu)
- **C++**: 17
- **Permissions**: Appropriate system-level permissions on the remote machine

## Installation

### Prerequisites
1. C++17 installed on both client and server machines
2. TCP/IP connectivity between client and server
3. Required port access through firewall/security settings

### Setup Instructions
```bash
# Clone the repository
git clone https://github.com/your-username/TCP_IP-Remote-Management.git

# Navigate to build directory
cd TCP_IP-Remote-Management/Build

# Build the project
make
```

## Usage

### Command Reference

1. **Get Process List**
   ```bash
   get-process
   ```

2. **Memory Usage Monitoring**
   ```bash
   get-mem <process-name>
   # or
   get-mem <process-id>
   ```

3. **CPU Usage Monitoring**
   ```bash
   get-cpu-usage <process-name>
   # or
   get-cpu-usage <process-id>
   ```

4. **Port Usage Information**
   ```bash
   get-ports-used <process-name>
   # or
   get-ports-used <process-id>
   ```

5. **Process Termination**
   ```bash
   kill <process-name> -15
   # or
   kill <process-id> -15
   ```

6. **Process Restart**
   ```bash
   restart-process <process-name>
   # or
   restart-process <process-id>
   ```

7. **Help Command**
   ```bash
   help
   ```

### Example Usage Scenarios

#### Monitor System Process
```bash
# Get memory usage for process ID 1234
get-mem 1234

# Check CPU usage for a process named "myprocess"
get-cpu-usage myprocess

# View ports used by process ID 1234
get-ports-used 1234
```

#### Process Management
```bash
# Terminate process ID 1234
kill 1234 -15

# Restart a process named "myprocess"
restart-process myprocess
```

## Security Considerations
- Ensure proper authentication mechanisms are in place
- Use secure network connections
- Regularly update access permissions
- Monitor and log all remote management activities

## Troubleshooting
- Verify network connectivity between client and server
- Check process permissions on the remote machine
- Ensure correct process names or IDs are used
- Review system logs for error messages




