#
# $Id: Makefile.am,v 1.8 2007-03-22 14:24:54 oconrad Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

if SAGA_UNICODE
AM_CXXFLAGS = -fPIC `wx-config --unicode=yes --cxxflags` -D_SAGA_LINUX -D_SAGA_UNICODE -D_TYPEDEF_BYTE -D_TYPEDEF_WORD $(DBGFLAGS) -I.. 
AM_LDFLAGS = -fPIC `wx-config --unicode=yes --libs` -lm
else
AM_CXXFLAGS = -fPIC `wx-config --unicode=no --cxxflags` -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD $(DBGFLAGS) -I..
AM_LDFLAGS = -fPIC `wx-config --unicode=no --libs` -lm
endif

bin_PROGRAMS = saga_cmd
saga_cmd_LDADD = ../saga_api/libsaga_api.la
saga_cmd_SOURCES =\
callback.cpp\
module_library.cpp\
saga_cmd.cpp\
callback.h\
module_library.h


