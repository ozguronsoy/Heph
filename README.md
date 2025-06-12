# Heph

[![Linux](https://github.com/ozguronsoy/Heph/actions/workflows/build_linux.yml/badge.svg)](https://github.com/ozguronsoy/Heph/actions/workflows/build_linux.yml)
[![Windows](https://github.com/ozguronsoy/Heph/actions/workflows/build_windows.yml/badge.svg)](https://github.com/ozguronsoy/Heph/actions/workflows/build_windows.yml)
[![Android](https://github.com/ozguronsoy/Heph/actions/workflows/build_android.yml/badge.svg)](https://github.com/ozguronsoy/Heph/actions/workflows/build_android.yml)

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

CMake options:

- ``-DHEPH_BUILD_STATIC=On`` builds static library.
- ``-DHEPH_BUILD_SHARED=On`` builds shared library.
- ``-DHEPH_BUILD_DOCS=On`` builds documentation.
- ``-DHEPH_BUILD_TESTS=On`` builds tests.

Build Options:
- ``-DHEPH_CL_DISABLE_COLORED_OUTPUT`` disables colored console output.