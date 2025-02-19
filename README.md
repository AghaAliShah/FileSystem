File System Simulation
This project implements a basic file system simulation in C++, supporting directory navigation, file management, symbolic links, and permission handling. It provides a hierarchical tree structure for organizing files and directories, with built-in support for serialization, searching, and file operations.

Features
📁 Directory Management
Create directories (mkdir <path>)
Change directories (cd <path>)
Print current working directory (pwd)
List directory contents (ls <path>)
Remove directories (rmdir <path>)


📄 File Management
Create files (touch <filename>)
Read file contents (cat <filename>)
Move files and directories (mv <source> <destination>)
Copy files and directories (cp <source> <destination>)
Delete files (rm <filename>)
Rename files or directories (rename <oldname> <newname>)


🔗 Symbolic Links
Create symbolic links (ln -s <target> <linkname>)

🔍 Search & Permissions
Search for files in a directory (find <filename>)
Search within file contents (grep <text>)
Display file details (stat <filename>)
Change file permissions (chmod <permissions> <filename>)
Change file owner (chown <newowner> <filename>)


💾 Persistence
Save file system to a file (savetofile <filename>)
Load file system from a file (loadfromfile <filename>)
Usage

Initialize the file system.
Use commands to create, modify, and manage files and directories.
Save and load file system state as needed.
Dependencies


Standard C++ libraries (iostream, fstream, vector, ctime, etc.)
This project provides a simple command-line-based file system that mimics Unix-like behavior, making it useful for understanding hierarchical file storage and file operations. 🚀


Author: Syed Agha Ali
