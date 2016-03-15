if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libimagery_svm.la
libimagery_svm_la_SOURCES =\
MLB_Interface.cpp\
svm.cpp\
svm_grids.cpp\
MLB_Interface.h\
svm.h\
svm_grids.h

libimagery_svm_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

