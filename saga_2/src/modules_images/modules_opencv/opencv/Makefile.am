#
# $Id: Makefile.am,v 1.2 2009-11-24 14:04:13 oconrad Exp $
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
AM_LDFLAGS         = -fPIC -shared
pkglib_LTLIBRARIES = libopencv.la
libopencv_la_SOURCES =\
MLB_Interface.cpp\
opencv.cpp\
opencv_fourier.cpp\
opencv_morphology.cpp\
opencv_svd.cpp\
MLB_Interface.h\
opencv.h\
opencv_fourier.h\
opencv_morphology.h\
opencv_svd.h

libopencv_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

