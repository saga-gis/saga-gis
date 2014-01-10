if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
DEP_DEFS = -fPIC `wx-config --unicode=yes --static=no --cxxflags` -D_SAGA_UNICODE $(DBGFLAGS)
DEP_LFLG = -fPIC -shared `wx-config --unicode=yes --static=no --libs`
else
DEP_DEFS = -fPIC `wx-config --unicode=no --static=no --cxxflags` $(DBGFLAGS)
DEP_LFLG = -fPIC -shared `wx-config --unicode=no --static=no --libs`
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = $(DEP_LFLG) -avoid-version
pkglib_LTLIBRARIES = libstatistics_kriging.la
libstatistics_kriging_la_SOURCES =\
_kriging_base.cpp\
_kriging_ordinary.cpp\
_kriging_ordinary_global.cpp\
_kriging_universal.cpp\
_kriging_universal_global.cpp\
kriging_base.cpp\
kriging_ordinary.cpp\
kriging_ordinary_global.cpp\
kriging_universal.cpp\
kriging_universal_global.cpp\
MLB_Interface.cpp\
semivariogram.cpp\
variogram_dialog.cpp\
_kriging_base.h\
_kriging_ordinary.h\
_kriging_ordinary_global.h\
_kriging_universal.h\
_kriging_universal_global.h\
kriging_base.h\
kriging_ordinary.h\
kriging_ordinary_global.h\
kriging_universal.h\
kriging_universal_global.h\
MLB_Interface.h\
semivariogram.h\
variogram_dialog.h

libstatistics_kriging_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(top_srcdir)/src/saga_core/saga_gdi/libsaga_gdi.la $(ADD_MLBS)
