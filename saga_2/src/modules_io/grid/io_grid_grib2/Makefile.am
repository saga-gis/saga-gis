#
# $Id: Makefile.am,v 1.1 2007-09-18 11:49:05 tschorr Exp $
#
DEP_INCS = -I/usr/local/lib/wx
DEP_DEFS = `wx-config --unicode=no --cxxflags`
DEP_LFLG = `wx-config --unicode=no --libs`
SAGA_SRC_DIR = /home/tschorr/TUM/geostep/saga/CVS/saga_2
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(SAGA_SRC_DIR)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) `wx-config --cxxflags`
AM_LDFLAGS         = -fPIC -shared $(DEP_LFLG) -lpq -ljasper
pkglib_LTLIBRARIES = libio_grid_grib2.la
libio_grid_grib2_la_SOURCES =\
grib2_import.cpp\
MLB_Interface.cpp\
grib2_import.h\
MLB_Interface.h

libio_grid_grib2_la_LIBADD = $(SAGA_SRC_DIR)/src/saga_core/saga_api/libsaga_api.la ./g2clib-1.0.4/libgrib2c.a


