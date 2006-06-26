CXX_DEFS  = `wx-config --cxxflags`
if X86_64
ARCHDEFS =  -fPIC -fpermissive
endif
AM_CXXFLAGS = $(CXX_DEFS) $(ARCHDEFS) -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -I..

#LIBS = `wx-config --libs` -pthread -L/usr/X11R6/lib -ldl -lm -Wl,--export-dynamic
#LIBS = `wx-config --libs` -lm -L/opt/gnome/lib -lpangox-1.0 -lsaga_api
LIBS = `wx-config --libs` -lm -L/opt/gnome/lib -lpangox-1.0
bin_PROGRAMS = saga_cmd

saga_cmd_SOURCES =\
callback.cpp\
module_library.cpp\
saga_cmd.cpp\
callback.h\
module_library.h

saga_cmd_LDADD = ../saga_api/libsaga_api.la
