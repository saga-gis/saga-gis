#
# $Id: Makefile.am,v 1.5 2007-02-13 12:49:27 tschorr Exp $
#
DEP_LIBS = -lproj
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
DEP_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared $(DEP_LIBS)
pkglib_LTLIBRARIES = libpj_proj4.la
libpj_proj4_la_SOURCES =\
MLB_Interface.cpp\
PROJ4_Base.cpp\
PROJ4_Grid.cpp\
PROJ4_Shapes.cpp\
MLB_Interface.h\
PROJ4_Base.h\
PROJ4_Grid.h\
PROJ4_Shapes.h

libpj_proj4_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

