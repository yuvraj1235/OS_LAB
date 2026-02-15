# my_ps - Custom Process Information Command

A custom command-line utility that merges the output of `ps aux` and `ps -eLf` commands into a single file for easy process and thread analysis.

##  Table of Contents
- [Overview](#overview)
- [How It Works](#how-it-works)

## Overview

`my_ps` is a C program that:
- Executes `ps aux` command (shows process information)
- Executes `ps -eLf` command (shows thread-level information)
- Merges both outputs into a single file (`merged.txt`)
- Displays the merged output in the terminal

### Step 2: Compile the Program

```bash
gcc my_ps.c -o my_ps
```

This creates an executable file named `my_ps`.

### Step 3: Test Locally

```bash
./my_ps
```

You should see the merged output displayed in your terminal.

### Step 4: Install System-Wide

#### Install to /usr/local/bin 

```bash
sudo cp my_ps /usr/local/bin/
sudo chmod +x /usr/local/bin/my_ps
```

### Verify Installation

```bash
# Try running from any directory
cd ~
my_ps
```

If successful, the command should execute and display merged process information.

##  How It Works

1. **Execute Commands**: Uses `system()` to run `ps aux` and `ps -eLf`
2. **Capture Output**: Redirects command output to text files (x1.txt, x2.txt)
3. **File Processing**: Opens files using `fopen()` in read mode
4. **Merge Data**: Reads content with `fgets()` and writes to merged.txt using `fprintf()`
5. **Display**: Shows the final merged.txt content using `cat` command
6. **Cleanup**: Closes all file handles with `fclose()`

## Learning Objectives

This project demonstrates:
- File I/O operations in C
- System calls using `system()`
- Process and thread information analysis
- Creating custom command-line utilities
- PATH environment variable management

