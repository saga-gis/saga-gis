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
pkglib_LTLIBRARIES = libsim_rivflow.la
libsim_rivflow_la_SOURCES =\
DataTrans.cpp\
GridComb.cpp\
LandFlow.cpp\
MLB_Interface.cpp\
RivBasin.cpp\
RivCourseImpr.cpp\
RivGridPrep.cpp\
DataTrans.h\
GridComb.h\
LandFlow.h\
MLB_Interface.h\
RivBasin.h\
RivCourseImpr.h\
RivGridPrep.h

libsim_rivflow_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

