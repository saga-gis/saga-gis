#
# $Id: Makefile.am,v 1.5 2007-02-13 12:49:18 tschorr Exp $
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
pkglib_LTLIBRARIES = libgrid_spline.la
libgrid_spline_la_SOURCES =\
Gridding_Spline_BA.cpp\
Gridding_Spline_Base.cpp\
Gridding_Spline_MBA.cpp\
Gridding_Spline_TPS_Global.cpp\
Gridding_Spline_TPS_Local.cpp\
Gridding_Spline_TPS_TIN.cpp\
MLB_Interface.cpp\
Thin_Plate_Spline.cpp\
Gridding_Spline_BA.h\
Gridding_Spline_Base.h\
Gridding_Spline_MBA.h\
Gridding_Spline_TPS_Global.h\
Gridding_Spline_TPS_Local.h\
Gridding_Spline_TPS_TIN.h\
MLB_Interface.h\
Thin_Plate_Spline.h

libgrid_spline_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

