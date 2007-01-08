DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) `wx-config --cxxflags`
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libtable_tools.la
libtable_tools_la_SOURCES =\
MLB_Interface.cpp\
Table_Create_Empty.cpp\
Table_Enumerate.cpp\
Table_Rotate.cpp\
MLB_Interface.h\
Table_Create_Empty.h\
Table_Enumerate.h\
Table_Rotate.h

libtable_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libtable_tools.so table_tools.so; popd
