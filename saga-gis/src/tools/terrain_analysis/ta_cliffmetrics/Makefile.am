if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -DCPU -Dx86
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libta_cliffmetrics.la
libta_cliffmetrics_la_SOURCES =\
CliffMetrics_tool.cpp\
TLB_Interface.cpp\
CliffMetrics/2di_point.cpp\
CliffMetrics/2di_shape.cpp\
CliffMetrics/2d_point.cpp\
CliffMetrics/2d_shape.cpp\
CliffMetrics/calc_curvature.cpp\
CliffMetrics/cell.cpp\
CliffMetrics/cell_landform.cpp\
CliffMetrics/coast.cpp\
CliffMetrics/coast_landform.cpp\
CliffMetrics/coast_polygon.cpp\
CliffMetrics/create_profiles.cpp\
CliffMetrics/delineation.cpp\
CliffMetrics/gis_raster.cpp\
CliffMetrics/gis_utils.cpp\
CliffMetrics/gis_vector.cpp\
CliffMetrics/hermite_cubic.cpp\
CliffMetrics/init_grid.cpp\
CliffMetrics/i_line.cpp\
CliffMetrics/line.cpp\
CliffMetrics/locate_clifftop.cpp\
CliffMetrics/locate_coast.cpp\
CliffMetrics/multi_line.cpp\
CliffMetrics/profile.cpp\
CliffMetrics/random_numbers.cpp\
CliffMetrics/raster_grid.cpp\
CliffMetrics/read_input.cpp\
CliffMetrics/smooth_grid.cpp\
CliffMetrics/smooth_line.cpp\
CliffMetrics/utils.cpp\
CliffMetrics/utils_global.cpp\
CliffMetrics/write_output.cpp\
CliffMetrics_tool.h\
CliffMetrics/2di_point.h\
CliffMetrics/2di_shape.h\
CliffMetrics/2d_point.h\
CliffMetrics/2d_shape.h\
CliffMetrics/cell.h\
CliffMetrics/cell_landform.h\
CliffMetrics/cliffmetrics.h\
CliffMetrics/coast.h\
CliffMetrics/coast_landform.h\
CliffMetrics/coast_polygon.h\
CliffMetrics/delineation.h\
CliffMetrics/hermite_cubic.h\
CliffMetrics/i_line.h\
CliffMetrics/line.h\
CliffMetrics/multi_line.h\
CliffMetrics/profile.h\
CliffMetrics/raster_grid.h

libta_cliffmetrics_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

