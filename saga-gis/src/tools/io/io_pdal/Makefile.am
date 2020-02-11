if DEBUG
DBGFLAGS = -g -DDEBUG
endif

if HAVE_PDAL

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version -lpdalcpp -lpdal_util
pkglib_LTLIBRARIES = libio_pdal.la
libio_pdal_la_SOURCES =\
pdal_reader.cpp\
TLB_Interface.cpp\
pdal_reader.h

libio_pdal_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

endif
