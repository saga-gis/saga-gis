if DEBUG
DBGFLAGS = -g -DDEBUG
endif

if HAVE_PDAL

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core $(PDAL_INC)
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS) $(PDAL_CPPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version $(PDAL_LIBS)
pkglib_LTLIBRARIES = libio_pdal.la
libio_pdal_la_SOURCES =\
pdal_driver.cpp\
pdal_reader.cpp\
pdal_writer_las.cpp\
TLB_Interface.cpp\
pdal_driver.h\
pdal_reader.h\
pdal_writer_las.h

libio_pdal_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

endif
