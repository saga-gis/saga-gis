#
# $Id: Makefile.am,v 1.4 2007-02-13 09:58:07 tschorr Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared 
pkglib_LTLIBRARIES = libta_preprocessor.la
libta_preprocessor_la_SOURCES =\
FillSinks.cpp\
MLB_Interface.cpp\
Pit_Eliminator.cpp\
Pit_Router.cpp\
FillSinks.h\
MLB_Interface.h\
Pit_Eliminator.h\
Pit_Router.h

libta_preprocessor_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

