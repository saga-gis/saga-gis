if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEP_DEFS = -fPIC `wx-config --unicode=yes --static=no --cxxflags`
DEP_LFLG = -fPIC -shared `wx-config --unicode=yes --static=no --libs`

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = $(DEP_LFLG) -avoid-version
pkglib_LTLIBRARIES = libtin_viewer.la
libtin_viewer_la_SOURCES =\
MLB_Interface.cpp\
tin_view_control.cpp\
tin_view_dialog.cpp\
tin_view_module.cpp\
MLB_Interface.h\
tin_view_control.h\
tin_view_dialog.h\
tin_view_module.h

libtin_viewer_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(top_srcdir)/src/saga_core/saga_gdi/libsaga_gdi.la $(ADD_MLBS)
