#
# $Id$
#
DEP_LIBS = -lproj
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core $(LOCAL_INCS)
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version $(DEP_LIBS) $(LOCAL_LIBS)
pkglib_LTLIBRARIES = libpj_proj4.la
libpj_proj4_la_SOURCES =\
MLB_Interface.cpp\
crs_assign.cpp\
crs_base.cpp\
crs_distance.cpp\
crs_grid_geogcoords.cpp\
crs_indicatrix.cpp\
crs_transform.cpp\
crs_transform_grid.cpp\
crs_transform_point.cpp\
crs_transform_pointcloud.cpp\
crs_transform_shapes.cpp\
crs_transform_utm.cpp\
gcs_graticule.cpp\
gcs_lon_range.cpp\
PROJ4_Base.cpp\
PROJ4_Grid.cpp\
PROJ4_Shapes.cpp\
MLB_Interface.h\
crs_assign.h\
crs_base.h\
crs_distance.h\
crs_grid_geogcoords.h\
crs_indicatrix.h\
crs_transform.h\
crs_transform_grid.h\
crs_transform_point.h\
crs_transform_pointcloud.h\
crs_transform_shapes.h\
crs_transform_utm.h\
gcs_graticule.h\
gcs_lon_range.h\
PROJ4_Base.h\
PROJ4_Grid.h\
PROJ4_Shapes.h

libpj_proj4_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
