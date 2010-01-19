#
# $Id: Makefile.am,v 1.9 2010-01-19 14:49:25 oconrad Exp $
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
pkglib_LTLIBRARIES = libgrid_discretisation.la
libgrid_discretisation_la_SOURCES =\
Grid_Classify_Supervised.cpp\
Grid_Cluster_Analysis.cpp\
Grid_FastSegments.cpp\
Grid_Segmentation.cpp\
Grid_Skeletonize.cpp\
MLB_Interface.cpp\
rga_basic.cpp\
Grid_Classify_Supervised.h\
Grid_Cluster_Analysis.h\
Grid_FastSegments.h\
Grid_Segmentation.h\
Grid_Skeletonize.h\
MLB_Interface.h\
rga_basic.h

libgrid_discretisation_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

