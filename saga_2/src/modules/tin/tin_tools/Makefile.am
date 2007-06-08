DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libtin_tools.la
libtin_tools_la_SOURCES =\
MLB_Interface.cpp\
TIN_Flow_Parallel.cpp\
TIN_Flow_Trace.cpp\
TIN_From_Grid.cpp\
TIN_From_Grid_Specific_Points.cpp\
TIN_From_Shapes.cpp\
TIN_Gradient.cpp\
TIN_To_Shapes.cpp\
MLB_Interface.h\
TIN_Flow_Parallel.h\
TIN_Flow_Trace.h\
TIN_From_Grid.h\
TIN_From_Grid_Specific_Points.h\
TIN_From_Shapes.h\
TIN_Gradient.h\
TIN_To_Shapes.h

libtin_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libtin_tools.so tin_tools.so; popd
