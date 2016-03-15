#
# $Id: Makefile.am 1613 2013-02-22 11:20:39Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libta_lighting.la
libta_lighting_la_SOURCES =\
HillShade.cpp\
MLB_Interface.cpp\
SolarRadiation.cpp\
topographic_correction.cpp\
topographic_openness.cpp\
view_shed.cpp\
Visibility_BASE.cpp\
Visibility_Point.cpp\
Visibility_Points.cpp\
HillShade.h\
MLB_Interface.h\
SolarRadiation.h\
topographic_correction.h\
topographic_openness.h\
view_shed.h\
Visibility_BASE.h\
Visibility_Point.h\
Visibility_Points.h

libta_lighting_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

