#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libriegl_io_rdb2_import.la
libriegl_io_rdb2_import_la_SOURCES =\
rdb2_import.cpp\
rdb2_import.h\
rdb2_info.cpp\
rdb2_info.h\
MLB_Interface.cpp\
MLB_Interface.h

# add rapidjson
CXX_INCS           += -I../libs/rapidjson/include

# add linux rdb
CXX_INCS           += -I../libs/rdblib-2.0.938-x86_64-linux-factory/interface/c
CXX_INCS           += -I../libs/rdblib-2.0.938-x86_64-linux-factory/interface/cpp
libriegl_io_rdb2_import_la_SOURCES += ../libs/rdblib-2.0.938-x86_64-linux-factory/interface/cpp/riegl/rdb.cpp
AM_CXXFLAGS        += -std=c++0x
AM_LDFLAGS         += -L../libs/rdblib-2.0.938-x86_64-linux-factory/library
AM_LDFLAGS       += -lrdb

libriegl_io_rdb2_import_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

