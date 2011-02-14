#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
if HAVE_CV
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core -I/usr/include/opencv
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version -lcv
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
endif
