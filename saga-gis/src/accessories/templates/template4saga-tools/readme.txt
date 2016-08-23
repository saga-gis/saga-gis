___________________________
>>> template4saga-tools <<<

This directory contains a SAGA tool library template including one initial tool.

Files:
./readme.txt
./my_tools_link2bin/.vc10.sln
./my_tools_link2src/.vc10.sln
./my_tool_library/Makefile
./my_tool_library/Makefile.am
./my_tool_library/TLB_Interface.cpp
./my_tool_library/my_tool.cpp
./my_tool_library/my_tool.h
./my_tool_library/my_tool_library.cbp
./my_tool_library/my_tool_library.layout
./my_tool_library/my_tool_library_link2bin.vcxproj
./my_tool_library/my_tool_library_link2bin.vcxproj.filters
./my_tool_library/my_tool_library_link2src.vcxproj
./my_tool_library/my_tool_library_link2src.vcxproj.filters

___________________________
Source code files

The tool library interface definition.
	./my_tool_library/TLB_Interface.cpp

The tool implementation. Just says 'Hello World' and prints a number.
	./my_tool_library/my_tool.cpp
	./my_tool_library/my_tool.h

___________________________
Linux compilation

For Linux compilation you have three options. All assume that you have installed SAGA somehow along with the related developement files (headers and shared objects).

This is a simple Makefile.
	./my_tool_library/Makefile

If you work with the complete SAGA sources you can take and modify this file to integrate your tool library into SAGA'S automake system. In this case you also have to place your project in SAGA's source code tree to modify '(SAGA_SOURCES_ROOT_DIR)/saga-gis/configure.ac'.
	./my_tool_library/Makefile.am

These files define a CodeBlocks (http://www.codeblocks.org/) project for the tool library template. Environment has been set up for a standard Linux installation using gcc. You might have to adjust some settings related to different paths on your system (e.g. '/usr/lib' instead of '/usr/local/lib').
	./my_tool_library/my_tool_library.cbp
	./my_tool_library/my_tool_library.layout

___________________________
Windows compilation

For the compilation on Windows you have two options, both requiring MS Visual C++ installed on your system. Any 'Express Edition' (these are free of charge) of version 10 (aka 2010) or later will do. Opening the solution files (*.sln) with versions later than 10 you will be prompted to upgrade the project file, which should work without any harm. The difference between the two solutions/projects is the way they link to SAGA.

1.) The very easy way is to link against the binary release version of SAGA just as you download it from the SAGA homepage. To let the project know where to find the necessary SAGA API files you have to set an environment variable pointing to its installation. The environment variable has to be named either SAGA_32 for the 32-bit version or SAGA_64 for the 64-bit version, e.g.:
	SET SAGA_32=C:\saga_2.3.1_win32\
or
	SET SAGA_64=C:\saga_2.3.1_x64\

The solution/project files are:	
	./my_tools_link2bin/.vc10.sln
	./my_tool_library/my_tool_library_link2bin.vcxproj
	./my_tool_library/my_tool_library_link2bin.vcxproj.filters

2.) The second option is a bit more sophisticated, because it needs SAGA to be compiled on your system. Although you have to do a few more preparations before you can start to develop your own tools, this might be your preferred choice for serious tool developments, in particular because it allows you to debug into SAGA's source codes (and even into other libraries like wxWidgets). Again you have to set an environment variable (this time only one!), pointing to the root directory ('saga-gis') of the SAGA source code tree, e.g.:
	SET SAGA=C:\saga_2.3.1_src\saga-gis\

The solution/project files are:	
	./my_tools_link2src/.vc10.sln
	./my_tool_library/my_tool_library_link2src.vcxproj
	./my_tool_library/my_tool_library_link2src.vcxproj.filters
