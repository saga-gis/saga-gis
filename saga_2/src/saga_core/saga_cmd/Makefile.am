#
# $Id: Makefile.am,v 1.7 2007-02-13 09:58:10 tschorr Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

if SAGA_UNICODE
AM_CXXFLAGS = -fPIC `wx-config --unicode=yes --cxxflags` -D_SAGA_LINUX -D_SAGA_UNICODE -D_TYPEDEF_BYTE -D_TYPEDEF_WORD $(DBGFLAGS) -I.. 
AM_LDFLAGS = -fPIC `wx-config --unicode=yes --libs` -lm -lpangox-1.0
else
AM_CXXFLAGS = -fPIC `wx-config --unicode=no --cxxflags` -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD $(DBGFLAGS) -I..
AM_LDFLAGS = -fPIC `wx-config --unicode=no --libs` -lm -lpangox-1.0
endif

bin_PROGRAMS = saga_cmd
saga_cmd_LDADD = ../saga_api/libsaga_api.la
saga_cmd_SOURCES =\
callback.cpp\
module_library.cpp\
saga_cmd.cpp\
callback.h\
module_library.h


