if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libgeostatistics_points.la
libgeostatistics_points_la_SOURCES =\
GSPoints_Semi_Variances.cpp\
MLB_Interface.cpp\
GSPoints_Semi_Variances.h\
MLB_Interface.h

libgeostatistics_points_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

