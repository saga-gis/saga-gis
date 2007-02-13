#
# $Id: Makefile.am,v 1.5 2007-02-13 09:58:06 tschorr Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared 
pkglib_LTLIBRARIES = libta_lighting.la
libta_lighting_la_SOURCES =\
HillShade.cpp\
MLB_Interface.cpp\
SADO_SolarRadiation.cpp\
SolarRadiation.cpp\
Visibility_Point.cpp\
HillShade.h\
MLB_Interface.h\
SADO_SolarRadiation.h\
SolarRadiation.h\
Visibility_Point.h

libta_lighting_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

