#
# $Id: Makefile.am,v 1.4 2007-02-13 09:58:01 tschorr Exp $
#
DEP_LIBS = -lgeotrans
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared $(DEP_LFLG) $(DEP_LIBS)
pkglib_LTLIBRARIES = libpj_geotrans.la
libpj_geotrans_la_SOURCES =\
GEOTRANS_Base.cpp\
GEOTRANS_Grid.cpp\
GEOTRANS_Shapes.cpp\
MLB_Interface.cpp\
GEOTRANS_Base.h\
GEOTRANS_Grid.h\
GEOTRANS_Shapes.h\
MLB_Interface.h

libpj_geotrans_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

