#
# $Id: Makefile.am,v 1.4 2007-02-13 09:57:49 tschorr Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared
pkglib_LTLIBRARIES = libgrid_discretisation.la
libgrid_discretisation_la_SOURCES =\
Grid_Classify_Supervised.cpp\
Grid_Cluster_Analysis.cpp\
Grid_FastSegments.cpp\
Grid_Segmentation.cpp\
Grid_Skeletonize.cpp\
MLB_Interface.cpp\
Grid_Classify_Supervised.h\
Grid_Cluster_Analysis.h\
Grid_FastSegments.h\
Grid_Segmentation.h\
Grid_Skeletonize.h\
MLB_Interface.h

libgrid_discretisation_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

