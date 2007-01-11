if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -shared $(DEP_LFLG) $(DEP_LIBS)
pkglib_LTLIBRARIES = libgeostatistics_kriging.la
libgeostatistics_kriging_la_SOURCES =\
Kriging_Base.cpp\
Kriging_Ordinary.cpp\
Kriging_Ordinary_Global.cpp\
Kriging_Universal.cpp\
Kriging_Universal_Global.cpp\
MLB_Interface.cpp\
Kriging_Base.h\
Kriging_Ordinary.h\
Kriging_Ordinary_Global.h\
Kriging_Universal.h\
Kriging_Universal_Global.h\
MLB_Interface.h

libgeostatistics_kriging_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

