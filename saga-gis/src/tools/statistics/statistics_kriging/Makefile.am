if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEP_DEFS = -fPIC `wx-config --unicode=yes --static=no --cxxflags`
DEP_LFLG = -fPIC -shared `wx-config --unicode=yes --static=no --libs`

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = $(DEP_LFLG) -avoid-version
pkglib_LTLIBRARIES = libstatistics_kriging.la
libstatistics_kriging_la_SOURCES =\
kriging_base.cpp\
kriging_ordinary.cpp\
kriging_regression.cpp\
kriging_simple.cpp\
kriging_universal.cpp\
MLB_Interface.cpp\
semivariogram.cpp\
variogram_dialog.cpp\
kriging_base.h\
kriging_ordinary.h\
kriging_regression.h\
kriging_simple.h\
kriging_universal.h\
MLB_Interface.h\
semivariogram.h\
variogram_dialog.h

libstatistics_kriging_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(top_srcdir)/src/saga_core/saga_gdi/libsaga_gdi.la $(ADD_MLBS)
