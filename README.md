# myfind

A parallelized file search utility written in C++17 as part of the course "Grundlagen verteilter Systeme". The application searches for multiple filenames simultaneously without executing external Linux utilities like `find`.

## Features

* **Process-Level Parallelism:** Forks an independent child process for every target filename.
* **POSIX Synchronization:** Atomic output writes via system-level calls to prevent interleaved `stdout` streams.
* **Zombie Prevention:** Parent process monitors and reaps all child processes upon completion.
* **C++17 Filesystem:** Native path resolution and directory traversal.

## Build Requirements

* C++17 compliant compiler (`g++` or `clang++`)
* `CMake` (v3.15+) or `make`

## Building the Project

### Using CMake
```bash
mkdir build && cd build
cmake ..
make
```

### Using Makefile
```bash
make        # Compiles the executable
make clean  # Cleans build artifacts
```

## Usage
```bash
./myfind [-R] [-i] <searchpath> <filename1> [filename2] ... [filenameN]
```

### Arguments & Options
<searchpath>: Relative or absolute target path.
<filename>: Plain string target filename(s).
-R: Enable recursive directory search (optional).
-i: Enable case-insensitive matching (optional).

### Example Output
```bash
$ ./myfind . main.cpp args.hpp
12345: main.cpp: /home/user/myfind/main.cpp
12346: args.hpp: /home/user/myfind/args.hpp
```