#
# $Id: Makefile.am,v 1.5 2007-02-13 12:49:32 tschorr Exp $
#
ADD_MLBS = ../ta_channels/libta_channels.la ../ta_hydrology/libta_hydrology.la ../ta_lighting/libta_lighting.la ../ta_morphometry/libta_morphometry.la ../ta_preprocessor/libta_preprocessor.la
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS)
AM_LDFLAGS         = -fPIC -shared 
pkglib_LTLIBRARIES = libta_compound.la
libta_compound_la_SOURCES =\
MLB_Interface.cpp\
TA_Standard.cpp\
MLB_Interface.h\
TA_Standard.h

libta_compound_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

