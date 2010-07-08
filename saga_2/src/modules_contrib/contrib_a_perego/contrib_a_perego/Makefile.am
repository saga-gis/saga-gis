#
# $Id: Makefile.am,v 1.2 2010-07-08 14:11:50 johanvdw Exp $
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
pkglib_LTLIBRARIES = libcontrib_a_perego.la
libcontrib_a_perego_la_SOURCES =\
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

libcontrib_a_perego_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

