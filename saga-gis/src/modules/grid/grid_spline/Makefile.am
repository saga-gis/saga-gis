#
# $Id$
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
pkglib_LTLIBRARIES = libgrid_spline.la
libgrid_spline_la_SOURCES =\
csa.c\
Gridding_Spline_BA.cpp\
Gridding_Spline_Base.cpp\
Gridding_Spline_CSA.cpp\
Gridding_Spline_MBA.cpp\
Gridding_Spline_MBA_Grid.cpp\
Gridding_Spline_TPS_Global.cpp\
Gridding_Spline_TPS_Local.cpp\
Gridding_Spline_TPS_TIN.cpp\
MLB_Interface.cpp\
csa.h\
Gridding_Spline_BA.h\
Gridding_Spline_Base.h\
Gridding_Spline_MBA.h\
Gridding_Spline_CSA.h\
Gridding_Spline_MBA_Grid.h\
Gridding_Spline_TPS_Global.h\
Gridding_Spline_TPS_Local.h\
Gridding_Spline_TPS_TIN.h\
MLB_Interface.h

libgrid_spline_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

