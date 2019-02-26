#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEP_DEFS = -fPIC `wx-config --unicode=yes --static=no --cxxflags` $(DBGFLAGS)
DEP_LFLG = -fPIC -shared `wx-config --unicode=yes --static=no --libs`

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version $(DEP_LFLG)
pkglib_LTLIBRARIES = libdev_tools.la
libdev_tools_la_SOURCES =\
arctoolbox.cpp\
MLB_Interface.cpp\
tl_extract.cpp\
tl_merge.cpp\
tl_union.cpp\
tool_counter.cpp\
tool_py_updater.cpp\
arctoolbox.h\
tl_extract.h\
tl_merge.h\
tl_union.h\
tool_counter.h\
tool_py_updater.h

libdev_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

