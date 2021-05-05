# TCP/IP demo application

Test application including a TCP client and server. Implemented for various programming languages,
operating systems and IDEs. Applications can generally be configured to run server or client only.

# C++

The demo is located inside the `tcpip-cpp` folder and 
can be configured by editing the `demo_config.h` file in the build directory after 
generating the build system with `CMake`.

## Build instructions 

Project template for building C++ and C projects with CMake and various different compilers
and IDEs. Install [CMake](https://cmake.org/install/) first.

All builds displayed here are for Debug configurations. To build for release, replace
`Debug` with `Release` in the folder names and and add `-DCMAKE_BUILD_TYPE=Release` 
to the CMake build command.

## Windows - GCC

### Using MinGW Makefiles

Install [MSYS2](https://www.msys2.org/) first. All command line steps here were done
in MinGW64. Set up MinGW64:

```sh
pacman -Syuuu
pacman -S git mingw-w64-x86_64-gdb mingw-w64-x86_64-make mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc
```

Then build the application with

```cpp
cd tcpip-cpp
mkdir build-Debug-MinGW64 && cd build-Debug-MinGW64
cmake .. -G "MinGW Makefiles"
cmake --build . -j
```

#### Using Visual Studio Code

Make sure you can build the application with MinGW64 like specified above. After that
there are good instructions on how to set up Visual Studio Code 
[here](https://code.visualstudio.com/docs/cpp/config-mingw). A workspace file is provided as well
to get started quickly.

#### Using Eclipse

Copy the `.project` and `.cproject` file found in `misc/eclipse` into the project root
and open the folder in Eclipse with `Open Project from Filesystem`. Build configurations and 
launch files for MinGW were provided. You have to generate the build system with CMake via 
command line first before you can Build, Run and Debug with Eclipse.

## Windows - MSVC

### Using Visual Studio

Install [Visual Studio](https://visualstudio.microsoft.com/).
You can generate Visual Studio project files with the following command 
(here, for Visual Studio 2019)

```cpp
cd tcpip-cpp
mkdir build-Debug-VS2019 && cd build-Debug-VS2019
cmake .. -G "Visual Studio 16 2019"
```

After that, a `.sln` project file is generated which you can open with Visual Studio 2019.

###  Using Visual Studio Code

Instructions can be found [here](https://code.visualstudio.com/docs/cpp/config-msvc).
Not tested yet.

## Windows - LLVM

Coming soon.

## Linux - GCC

### Using GCC and Unix Makefiles

Instructions for Ubuntu, adapt accordingly
Install required packages and tools first

```cpp
sudo apt-get update
sudo apt-get install build-essential gdb
```

Build the project like this: 

```cpp
cd tcpip-cpp
mkdir build-Debug-Unix
cd build-Debug-Unix
cmake .. 
cmake --build . -j
```

#### Using Eclipse

Copy the `.project` and `.cproject` file found in `misc/eclipse` into the project root
and open the folder in Eclipse with `Open Project from Filesystem`. Build configurations and 
launch files for Unix systems were provided. You have to generate the build system with CMake via 
command line first before you can Build, Run and Debug with Eclipse.

#### Using Visual Studio Code

Make sure you can build the application like specified above. After that
there are good instructions on how to set up Visual Studio Code 
[here](https://code.visualstudio.com/docs/cpp/config-linux)

## Linux - LLVM

Coming soon.

# C

The demo is located inside the `tcpip-c` folder.It can be configured like the C++ demo
by editing the `demo_config.h` file in the build folder after build system generation with `CMake`.

## Windows - GCC

### Using MinGW Makefiles

Perform the same preparation steps as specified in the C++ chapter for MinGW Makefiles,
then build the application with

```cpp
cd tcpip-c
mkdir build-Debug-MinGW64 && cd build-Debug-MinGW64
cmake .. -G "MinGW Makefiles"
cmake --build . -j
```

## Linux - GCC

### Using Makefiles

Perform the preparation steps as specified above for Unix Makefiles,
then build the application with

```cpp
cd tcpip-c
mkdir build-Debug-Unix && cd build-Debug-Unix
cmake ..
cmake --build . -j
```

# Java

If [Gradle](https://gradle.org/) is installed, you can run the demo with the command

```sh
./gradlew run
```

# Python

The demo can be configured via `config.py` file.

You can run the demo by executing the `main.py` file. Replaced `python3` with `py` on Windows.

```sh
python3 main.py
```

# Go

# Rust







