#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libtable_tools.la
libtable_tools_la_SOURCES =\
Join_Tables.cpp\
MLB_Interface.cpp\
Table_Create_Empty.cpp\
Table_Enumerate.cpp\
Table_Rotate.cpp\
Join_Tables.h\
MLB_Interface.h\
Table_Create_Empty.h\
Table_Enumerate.h\
Table_Rotate.h

libtable_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

