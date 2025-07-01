# file_operation
A custom command-line utility implemented in C, mimicking the functionalities of the "ls" command on Unix-like systems.

Target:

Write a C program that run on Ubuntu 22 with some function:

    List all file/folder in current directory.

    Print:

        Name

        Type (File/Folder)

        Owner (username)

        Last time edited

Approach:
 -List files in the current directory

    Use: opendir(), readdir(), closedir()

    Loop through entries (dirent->d_name), skip "." and ".."

 -Get detailed information for each file

    Use: stat() to retrieve info such as:

        Type (use S_ISDIR(), S_ISREG())

        Last modified time (st_mtime)

        UID → Owner name (via getpwuid())

 -Convert UID to username

    Use getpwuid(st_uid) from <pwd.h>

 -Convert time to human-readable string

    Use localtime() and strftime()

 -Print in a clear format

    Use printf() in a simple table layout

