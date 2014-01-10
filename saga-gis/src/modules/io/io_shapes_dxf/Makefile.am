#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libio_shapes_dxf.la
libio_shapes_dxf_la_SOURCES =\
dxf_import.cpp\
MLB_Interface.cpp\
./dxflib/dl_dxf.cpp\
./dxflib/dl_writer_ascii.cpp\
dxf_import.h\
MLB_Interface.h\
./dxflib/dl_attributes.h\
./dxflib/dl_codes.h\
./dxflib/dl_creationadapter.h\
./dxflib/dl_creationinterface.h\
./dxflib/dl_dxf.h\
./dxflib/dl_entities.h\
./dxflib/dl_exception.h\
./dxflib/dl_extrusion.h\
./dxflib/dl_writer.h\
./dxflib/dl_writer_ascii.h

libio_shapes_dxf_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

