#
# $Id: Makefile.am 1537 2012-11-19 20:05:19Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libsim_erosion.la
libsim_erosion_la_SOURCES =\
MMF_SAGA.cpp\
MLB_Interface.cpp\
MMF_SAGA.h\
MLB_Interface.h

libsim_erosion_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

