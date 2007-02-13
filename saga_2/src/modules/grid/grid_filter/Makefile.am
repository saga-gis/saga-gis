#
# $Id: Makefile.am,v 1.4 2007-02-13 09:57:51 tschorr Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared
pkglib_LTLIBRARIES = libgrid_filter.la
libgrid_filter_la_SOURCES =\
Filter.cpp\
Filter_3x3.cpp\
Filter_Gauss.cpp\
Filter_LoG.cpp\
Filter_Multi_Dir_Lee.cpp\
FilterClumps.cpp\
MLB_Interface.cpp\
Filter.h\
Filter_3x3.h\
Filter_Gauss.h\
Filter_LoG.h\
Filter_Multi_Dir_Lee.h\
FilterClumps.h\
MLB_Interface.h

libgrid_filter_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

