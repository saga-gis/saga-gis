if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libio_riegl_rdb.la
libio_riegl_rdb_la_SOURCES =\
rdb2_import.cpp\
rdb2_info.cpp\
TLB_Interface.cpp\
rdb2_import.h\
rdb2_info.h

# add rdb-cpp interface
libio_riegl_rdb_la_SOURCES += ../libs/rdblib-2.0.938-x86_64-linux-factory/interface/cpp/riegl/rdb.cpp

# add rapidjson
CXX_INCS           += -I./rapidjson/include

# add linux rdb
CXX_INCS           += -I../libs/rdblib-2.0.938-x86_64-linux-factory/interface/c
CXX_INCS           += -I../libs/rdblib-2.0.938-x86_64-linux-factory/interface/cpp
AM_CXXFLAGS        += -std=c++0x
AM_LDFLAGS         += -L../libs/rdblib-2.0.938-x86_64-linux-factory/library
AM_LDFLAGS         += -lrdb

libio_riegl_rdb_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
