if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEP_DEFS = -fPIC `wx-config --unicode=yes --static=no --cxxflags`
DEP_LFLG = -fPIC -shared `wx-config --unicode=yes --static=no --libs`

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = $(DEP_LFLG) -avoid-version
pkglib_LTLIBRARIES = libpointcloud_viewer.la
libpointcloud_viewer_la_SOURCES =\
MLB_Interface.cpp\
points_view_control.cpp\
points_view_dialog.cpp\
points_view_extent.cpp\
points_view_module.cpp\
MLB_Interface.h\
points_view_control.h\
points_view_dialog.h\
points_view_extent.h\
points_view_module.h

libpointcloud_viewer_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(top_srcdir)/src/saga_core/saga_gdi/libsaga_gdi.la $(ADD_MLBS)
