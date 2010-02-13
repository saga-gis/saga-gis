#
# $Id: Makefile.am,v 1.7 2010-02-13 22:38:44 reklov_w Exp $
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
pkglib_LTLIBRARIES = libgrid_filter.la
libgrid_filter_la_SOURCES =\
Filter.cpp\
Filter_3x3.cpp\
Filter_Gauss.cpp\
Filter_LoG.cpp\
Filter_Majority.cpp\
Filter_Multi_Dir_Lee.cpp\
Filter_Terrain_SlopeBased.cpp\
FilterClumps.cpp\
MLB_Interface.cpp\
Filter.h\
Filter_3x3.h\
Filter_Gauss.h\
Filter_LoG.h\
Filter_Majority.h\
Filter_Multi_Dir_Lee.h\
Filter_Terrain_SlopeBased.h\
FilterClumps.h\
MLB_Interface.h

libgrid_filter_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

