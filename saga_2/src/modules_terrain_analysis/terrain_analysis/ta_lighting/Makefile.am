#
# $Id: Makefile.am,v 1.9 2009-09-01 14:12:30 oconrad Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared 
pkglib_LTLIBRARIES = libta_lighting.la
libta_lighting_la_SOURCES =\
HillShade.cpp\
MLB_Interface.cpp\
SADO_SolarRadiation.cpp\
SolarRadiation.cpp\
topographic_correction.cpp\
view_shed.cpp\
Visibility_Point.cpp\
HillShade.h\
MLB_Interface.h\
SADO_SolarRadiation.h\
SolarRadiation.h\
topographic_correction.h\
view_shed.h\
Visibility_Point.h

libta_lighting_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

