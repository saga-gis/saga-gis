#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

MLIBPATH = "MODULE_LIBRARY_PATH=\"$(libdir)/saga\""
MSHAREPATH = "SHARE_PATH=\"$(prefix)/share/saga\""
BASEFLAGS = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD $(DBGFLAGS) -I.. -D$(MLIBPATH) -D$(MSHAREPATH) 

AM_CXXFLAGS = -fPIC `wx-config --unicode=yes --cxxflags base` $(BASEFLAGS) $(GOMPFLAGS)
AM_LDFLAGS = -fPIC `wx-config --unicode=yes --libs base` -lm


bin_PROGRAMS = saga_cmd
saga_cmd_LDADD = ../saga_api/libsaga_api.la
saga_cmd_SOURCES =\
callback.cpp\
config.cpp\
tool.cpp\
saga_cmd.cpp\
callback.h\
config.h\
tool.h

SUBDIRS = man
