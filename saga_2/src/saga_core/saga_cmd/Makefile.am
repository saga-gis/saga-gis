CXX_DEFS  = `wx-config --cxxflags`
if X86_64
ARCHDEFS =  -fPIC -fpermissive
endif
AM_CXXFLAGS = $(CXX_DEFS) $(ARCHDEFS) -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -I..
AM_LDFLAGS=`wx-config --libs` -lm -lpangox-1.0
bin_PROGRAMS = saga_cmd
saga_cmd_LDADD = ../saga_api/libsaga_api.la
saga_cmd_SOURCES =\
callback.cpp\
module_library.cpp\
saga_cmd.cpp\
callback.h\
module_library.h


