File System Simulation

__________________________________________________________________

This project implements a basic file system simulation in C++, supporting:

✅ Directory navigation

✅ File management

✅ Symbolic links

✅ Permission handling

It provides a hierarchical tree structure for organizing files and directories, with built-in support for serialization, searching, and file operations.

__________________________________________________________________


📁 Directory Management

Create directories → mkdir <path>

Change directories → cd <path>

Print current working directory → pwd

List directory contents → ls <path>

Remove directories → rmdir <path>

__________________________________________________________________

📄 File Management

Create files → touch <filename>

Read file contents → cat <filename>

Move files and directories → mv <source> <destination>

Copy files and directories → cp <source> <destination>

Delete files → rm <filename>

Rename files or directories → rename <oldname> <newname>

__________________________________________________________________

🔗 Symbolic Links

Create symbolic links → ln -s <target> <linkname>

__________________________________________________________________

🔍 Search & Permissions

Search for files in a directory → find <filename>

Search within file contents → grep <text>

Display file details → stat <filename>

Change file permissions → chmod <permissions> <filename>

Change file owner → chown <newowner> <filename>

__________________________________________________________________

💾 Persistence

Save file system to a file → savetofile <filename>

Load file system from a file → loadfromfile <filename>

__________________________________________________________________

📌 Usage

Initialize the file system

Use commands to create, modify, and manage files and directories

Save and load file system state as needed

__________________________________________________________________

🛠️ Dependencies

Standard C++ libraries:

iostream, fstream, vector, ctime, etc.

This project provides a simple command-line-based file system that mimics Unix-like behavior, making it useful for learning about hierarchical file storage and file operations. 🚀

__________________________________________________________________
__________________________________________________________________

📝 Author:

Syed Agha Ali

__________________________________________________________________
__________________________________________________________________
