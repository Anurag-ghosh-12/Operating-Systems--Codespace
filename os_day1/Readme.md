# Linux Command Reference Guide

This guide serves as a comprehensive reference for commonly used Linux commands. Each command is explained in detail, with examples and options to help you understand their usage effectively.

---

### 1. `man` - Manual Pages
**Description:** Displays the manual pages for other commands, providing comprehensive details and usage examples.

**Usage:**
- `man man` – Displays the manual for the `man` command itself.
- `man ls` – Shows the manual for the `ls` command.

**Tip:** Use `/keyword` to search within the manual and `q` to exit.

---

### 2. `pwd` - Print Working Directory
**Description:** Displays the current directory you are in.

**Usage:**
- `pwd` – Outputs the full path of the current working directory.

---

### 3. `ls` - List Directory Contents
**Description:** Lists files and directories.

**Usage:**
- `ls /` – Lists contents of the root directory.
- `ls -l /` – Lists contents with detailed info (permissions, owner, size, etc.).
- `ls -la` – Lists all files, including hidden ones.

---

### 4. `mkdir` - Make Directories
**Description:** Creates new directories.

**Usage:**
- `mkdir temp` – Creates a directory named `temp`.
- `mkdir -p temp/abc` – Creates directory `abc` inside `temp`, creating parent directories as needed.

---

### 5. `touch` - Create Empty Files
**Description:** Creates an empty file or updates the timestamp of an existing file.

**Usage:**
- `touch abc.xyz` – Creates an empty file named `abc.xyz`.
- `touch -t 202401011200 abc.xyz` – Sets the timestamp of `abc.xyz` to January 1, 2024, at 12:00.

---

### 6. `cd` - Change Directory
**Description:** Changes the current directory.

**Usage:**
- `cd /tmp` – Changes to the `/tmp` directory.
- `cd ..` – Moves one level up in the directory structure.
- `cd ~` – Moves to the home directory.

---

### 7. `cp` - Copy Files and Directories
**Description:** Copies files and directories.

**Usage:**
- `cp xyz abc` – Copies file `xyz` to `abc`.
- `cp xyz temp/` – Copies `xyz` into the `temp` directory.
- `cp -r temp /tmp/` – Recursively copies the `temp` directory to `/tmp/`.

---

### 8. `mv` - Move or Rename Files
**Description:** Moves or renames files and directories.

**Usage:**
- `mv xyz abc` – Renames `xyz` to `abc`.
- `mv xyz temp/` – Moves `xyz` into the `temp` directory.

---

### 9. `rm` - Remove Files and Directories
**Description:** Deletes files and directories.

**Usage:**
- `rm abc.xyz` – Deletes the file `abc.xyz`.
- `rm -r temp` – Recursively deletes the `temp` directory.
- `rm -f abc.xyz` – Forces deletion without prompting.

**Caution:** Use with care, especially `rm -rf` which can delete critical system files.

---

### 10. `dd` - Device Copy
**Description:** Copies data from one device/file to another.

**Usage:**
- `dd if=abc of=xyz` – Copies content from `abc` to `xyz`.
- `dd if=/dev/sda of=backup.img` – Creates an image of the disk `/dev/sda`.

---

### 11. `ssh` - Secure Shell
**Description:** Opens a secure shell session with a remote server.

**Usage:**
- `ssh -X manas@10.2.1.40` – Connects to the server at `10.2.1.40` as user `manas` with X forwarding.

---

### 12. `scp` - Secure Copy
**Description:** Securely copies files between local and remote systems.

**Usage:**
- `scp abc.txt manas@10.2.1.40:~/` – Copies `abc.txt` to the home directory of `manas` on the remote server.

---

### 13. `sudo` - Superuser Do
**Description:** Executes commands with superuser privileges.

**Usage:**
- `sudo bash` – Opens a shell with superuser privileges.
- `sudo apt update` – Runs the update command as superuser.

---

### 14. `ping` - Network Reachability Test
**Description:** Checks the connectivity to a remote host.

**Usage:**
- `ping 10.2.1.40` – Pings the IP address `10.2.1.40`.
- `ping www.google.com` – Pings Google's server.

---

### 15. `ifconfig` - Network Interface Configuration (Deprecated)
**Description:** Configures network interfaces.

**Usage:**
- `ifconfig` – Displays all active network interfaces.

**Note:** You may need to install `net-tools` for this command.

---

### 16. `netstat` - Network Statistics
**Description:** Displays network connections, routing tables, and interface statistics.

**Usage:**
- `netstat -rn` – Displays the routing table in numeric form.

---

### 17. `route` - Show/Manipulate IP Routing Table
**Description:** Displays or modifies the IP routing table.

**Usage:**
- `route` – Displays the current routing table.

---

### 18. `ip` - Show/Modify Network Configuration
**Description:** Displays and manages network interfaces and routing.

**Usage:**
- `ip addr show` – Shows all IP addresses assigned to network interfaces.

---

### 19. `vim` and `nano` - Text Editors
**Description:**
- `vim`: Advanced text editor with extensive features.
- `nano`: Simple, user-friendly text editor.

**Usage:**
- `vim file.txt` – Opens `file.txt` in Vim.
- `nano file.txt` – Opens `file.txt` in Nano.

---

### 20. `gcc` - GNU Compiler Collection
**Description:** Compiles C programs.

**Usage:**
- `gcc program.c -o program` – Compiles `program.c` into an executable named `program`.

---

### 21. `gdb` - GNU Debugger
**Description:** Debugs programs written in C/C++.

**Usage:**
- `gdb program` – Starts debugging the `program`.

---

### 22. `gas` - GNU Assembler
**Description:** Assembles assembly language source files.

**Usage:**
- `as file.s -o file.o` – Assembles `file.s` into an object file `file.o`.

---

### 23. `chown` - Change File Ownership
**Description:** Changes the owner of a file or directory.

**Usage:**
- `chown manas abc.xyz` – Changes the owner of `abc.xyz` to `manas`.
- `chown -R manas temp` – Recursively changes ownership of `temp` directory and its contents.

---

### 24. `chmod` - Change File Permissions
**Description:** Changes file permissions.

**Usage:**
- `chmod 744 abc.xyz` – Sets read, write, execute for owner; read for group and others.
- `chmod -R 777 temp` – Grants full permissions to everyone for the `temp` directory and its contents.

---

### 25. `ps` - Process Status
**Description:** Displays currently running processes.

**Usage:**
- `ps -ef` – Shows all running processes in full-format listing.

---

### 26. `kill` - Terminate Processes
**Description:** Terminates processes by PID.

**Usage:**
- `kill 10234` – Sends a termination signal to process with PID 10234.
- `kill -9 10234` – Forcefully kills process with PID 10234.

---

**Final Note:**
To explore any command further, always refer to its manual page using `man [command]`. Understanding these commands will enhance your Linux proficiency and system management skills.

