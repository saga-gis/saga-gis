DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) `wx-config --cxxflags`
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libcontrib_t_wutzler.la
libcontrib_t_wutzler_la_SOURCES =\
CompleteLinkage.cpp\
DebugStream.cpp\
GridCalcError.cpp\
MLB_Interface.cpp\
NoDataValueError.cpp\
Profiler.cpp\
rgbOverlay.cpp\
tomaschTool.cpp\
CompleteLinkage.h\
DebugStream.h\
GridCalcError.h\
MLB_Interface.h\
NoDataValueError.h\
Profiler.h\
rgbOverlay.h\
tomaschTool.h

libcontrib_t_wutzler_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libcontrib_t_wutzler.so contrib_t_wutzler.so; popd
