# Halite Environment/Engine

## Setup

Windows: install CMake into a path without spaces (NOT the default path), as well as the Visual Studio Build Tools (2017 or later).

Linux/MacOS: install a compiler (GCC or Clang) with C++17 support.

All platforms:

    cmake .
    make  # Use make -j4 if your CPU has 4 threads, etc.
