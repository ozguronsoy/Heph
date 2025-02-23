# Heph

- [Introduction](#introduction)
- [Setup](#setup)
    - [Dependencies](#dependencies)
    - [Build](#build)
- [Documentation](https://ozguronsoy.github.io/Heph/)<br>

## Introduction

Heph is a modern cross-platform C++ library that provides common components that are frequently used in most projects, such as:

- ``Exceptions`` for detailed error handling.
- ``Events`` for managing callback methods with custom parameters.
- Wrapper class for generating/managing native UUIDs.


## Setup

### Dependencies

All Platforms:
- C++23 or higher.
- CMake 3.28.0 or higher.
- Doxygen 1.11.0 or higher for building documentation.
- GTest for building tests.

Linux:
- libuuid.


### Build

Build options:

- ``-DHEPH_BUILD_STATIC=On`` to build static library.
- ``-DHEPH_BUILD_SHARED=On`` to build shared library.
- ``-DHEPH_BUILD_DOCS=On`` to build documentation.
- ``-DHEPH_BUILD_TESTS=On`` to build tests.
- ``-DCMAKE_CXX_FLAGS="-DHEPH_CL_DISABLE_COLORED_OUTPUT"`` to disable colored output for consoles that does not support ANSI escape codes.