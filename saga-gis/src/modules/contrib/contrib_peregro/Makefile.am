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
pkglib_LTLIBRARIES = libcontrib_perego.la
libcontrib_perego_la_SOURCES =\
A1WiTh.cpp\
A2WiTh.cpp\
A3WiTh.cpp\
AvWiMa1.cpp\
AvWiMa2.cpp\
destriping1.cpp\
destriping2.cpp\
directional1.cpp\
MLB_Interface.cpp\
A1WiTh.h\
A2WiTh.h\
A3WiTh.h\
AvWiMa1.h\
AvWiMa2.h\
destriping1.h\
destriping2.h\
directional1.h\
MLB_Interface.h

libcontrib_perego_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

