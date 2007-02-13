#
# $Id: Makefile.am,v 1.5 2007-02-13 12:49:25 tschorr Exp $
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
pkglib_LTLIBRARIES = libio_esri_e00.la
libio_esri_e00_la_SOURCES =\
ESRI_E00_Import.cpp\
MLB_Interface.cpp\
e00compr/cpl_conv.c\
e00compr/cpl_error.c\
e00compr/cpl_vsisimple.c\
e00compr/e00read.c\
e00compr/e00write.c\
ESRI_E00_Import.h\
MLB_Interface.h\
e00compr/cpl_conv.h\
e00compr/cpl_error.h\
e00compr/cpl_port.h\
e00compr/cpl_vsi.h\
e00compr/e00compr.h

libio_esri_e00_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

