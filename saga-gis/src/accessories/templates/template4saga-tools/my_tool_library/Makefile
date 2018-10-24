# Project: my_tool_library
# Linux GCC Makefile

LIB_NAME  = libmy_tool_library.so

SAGA_ROOT = /usr/local

CXX_INCS  = -I"$(SAGA_ROOT)/include/saga/saga_core"
CXX_DEFS  = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_FLGS  = -fPIC -DPIC -fopenmp -O2
CXXFLAGS  = $(CXX_INCS) $(CXX_DEFS) $(CXX_FLGS)
LNKFLAGS  = -fPIC -DPIC -shared
LNKLIBS   = -L"$(SAGA_ROOT)/lib" -lgomp -lsaga_api

CPP       = g++
LD        = g++

OBJ       = TLB_Interface.o my_tool.o


#########################################

.PHONY: all all-before all-after clean clean-custom

all: all-before $(LIB_NAME) all-after

clean: clean-custom
	rm -f $(OBJ) $(LIB_NAME)

$(LIB_NAME): $(OBJ)
	$(LD) $(LNKFLAGS) $(OBJ) $(LNKLIBS) -o $(LIB_NAME)

	
#########################################

TLB_Interface.o: TLB_Interface.cpp
	$(CPP) -c TLB_Interface.cpp -o TLB_Interface.o $(CXXFLAGS)

my_tool.o: my_tool.cpp
	$(CPP) -c my_tool.cpp -o my_tool.o $(CXXFLAGS)
