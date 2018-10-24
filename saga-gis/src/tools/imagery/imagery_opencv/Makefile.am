#
# $Id: Makefile.am 1856 2013-10-17 09:46:51Z oconrad $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

if HAVE_CV
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core -I/usr/include/opencv
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version `pkg-config opencv --libs`
pkglib_LTLIBRARIES = libimagery_opencv.la
libimagery_opencv_la_SOURCES =\
MLB_Interface.cpp\
opencv.cpp\
opencv_fourier.cpp\
opencv_ml.cpp\
opencv_ml_tools.cpp\
opencv_morphology.cpp\
opencv_nnet.cpp\
opencv_stereo_match.cpp\
opencv_svd.cpp\
MLB_Interface.h\
opencv.h\
opencv_fourier.h\
opencv_ml.h\
opencv_morphology.h\
opencv_nnet.h\
opencv_stereo_match.h\
opencv_svd.h

libimagery_opencv_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
endif
