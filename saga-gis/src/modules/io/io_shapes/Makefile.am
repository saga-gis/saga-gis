#
# $Id: Makefile.am 1373 2012-04-13 09:54:30Z oconrad $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEP_DEFS = -fPIC `wx-config --unicode=yes --static=no --cxxflags`
DEP_LFLG = -fPIC -shared `wx-config --unicode=yes --static=no --libs`

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version $(DEP_LFLG)
pkglib_LTLIBRARIES = libio_shapes.la
libio_shapes_la_SOURCES =\
atlas_bna.cpp\
generate.cpp\
gpx.cpp\
gstat.cpp\
MLB_Interface.cpp\
pointcloud_from_file.cpp\
pointcloud_from_text_file.cpp\
pointcloud_to_text_file.cpp\
stl.cpp\
surfer_bln.cpp\
svg.cpp\
wasp_map.cpp\
wktb.cpp\
xyz.cpp\
atlas_bna.h\
generate.h\
gpx.h\
gstat.h\
MLB_Interface.h\
pointcloud_from_file.h\
pointcloud_from_text_file.h\
pointcloud_to_text_file.h\
stl.h\
surfer_bln.h\
svg.h\
wasp_map.h\
wktb.h\
xyz.h

libio_shapes_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

