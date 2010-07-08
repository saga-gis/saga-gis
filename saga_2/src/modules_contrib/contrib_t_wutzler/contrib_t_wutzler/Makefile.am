#
# $Id: Makefile.am,v 1.7 2010-07-08 14:11:50 johanvdw Exp $
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
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libcontrib_t_wutzler.la
libcontrib_t_wutzler_la_SOURCES =\
CompleteLinkage.cpp\
DebugStream.cpp\
GridCalcError.cpp\
MLB_Interface.cpp\
NoDataValueError.cpp\
rgbOverlay.cpp\
tomaschTool.cpp\
CompleteLinkage.h\
DebugStream.h\
GridCalcError.h\
MLB_Interface.h\
NoDataValueError.h\
rgbOverlay.h\
tomaschTool.h

libcontrib_t_wutzler_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

