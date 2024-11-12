___________________________
>>> template4saga-tools <<<

This directory contains a SAGA tool library template including one initial tool.

Files:
./readme.txt
./my_tool_library/CMakeLists.txt
./my_tool_library/TLB_Interface.cpp
./my_tool_library/my_tool.cpp
./my_tool_library/my_tool.h

___________________________
Source code files

The tool library interface definition.
	./my_tool_library/TLB_Interface.cpp

The tool implementation. Just says 'Hello World' and prints a number.
	./my_tool_library/my_tool.cpp
	./my_tool_library/my_tool.h

___________________________
Configuration and building

Project configuration for your building environment expects CMake installed on your system (see https://cmake.org/ for details). In the simplest case the building procedure can then look like this (example for Linux):

> cd ~/my_tool_library
> mkdir build
> cd build
> cmake ..
> make

Under Windows you can define an environment variable 'SAGA' pointing to your SAGA installation folder, which allows CMake directly to find the dependencies, e.g.:

> cd C:\my_tool_library
> mkdir build
> cd build
> SET SAGA=C:\saga-9.7.0
> cmake ..
> ...
