#
# $Id: Makefile.am,v 1.10 2009-07-19 12:32:28 reklov_w Exp $
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
pkglib_LTLIBRARIES = libio_shapes.la
libio_shapes_la_SOURCES =\
atlas_bna.cpp\
generate.cpp\
gstat.cpp\
MLB_Interface.cpp\
pointcloud_from_file.cpp\
stl.cpp\
surfer_bln.cpp\
xyz.cpp\
wasp_map.cpp\
atlas_bna.h\
generate.h\
gstat.h\
MLB_Interface.h\
point_cloud_from_file.h\
stl.h\
surfer_bln.h\
wasp_map.h\
xyz.h

libio_shapes_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

