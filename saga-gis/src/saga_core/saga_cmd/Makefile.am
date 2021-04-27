#
# $Id$
#
if DEBUG
DBGFLAGS    = -g -DDEBUG
endif

MTOOLSPATH  = "TOOLS_PATH=\"$(libdir)/saga\""
MSHAREPATH  = "SHARE_PATH=\"$(prefix)/share/saga\""
BASEFLAGS   = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D$(MTOOLSPATH) -D$(MSHAREPATH) -I..

AM_CXXFLAGS = -fPIC `wx-config --unicode=yes --cxxflags base` $(BASEFLAGS) $(GOMPFLAGS) $(DBGFLAGS)
AM_LDFLAGS  = -fPIC `wx-config --unicode=yes --libs base` -lm

bin_PROGRAMS = saga_cmd

saga_cmd_SOURCES =\
callback.cpp\
config.cpp\
tool.cpp\
saga_cmd.cpp\
callback.h\
config.h\
tool.h

saga_cmd_LDADD = ../saga_api/libsaga_api.la

SUBDIRS = man
