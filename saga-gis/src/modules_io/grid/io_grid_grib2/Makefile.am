#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -DUSE_JPEG2000 -DUSE_PNG
CXX_INCS           = -I$(top_srcdir)/src/saga_core $(LOCAL_INCS)
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
#AM_LDFLAGS         = -fPIC -shared -avoid-version -lpq -ljasper
AM_LDFLAGS         = -fPIC -shared -avoid-version $(LOCAL_LIBS) -ljasper
pkglib_LTLIBRARIES = libio_grid_grib2.la
libio_grid_grib2_la_SOURCES =\
./g2clib-1.0.4/cmplxpack.c\
./g2clib-1.0.4/compack.c\
./g2clib-1.0.4/comunpack.c\
./g2clib-1.0.4/dec_jpeg2000.c\
./g2clib-1.0.4/dec_png.c\
./g2clib-1.0.4/drstemplates.c\
./g2clib-1.0.4/enc_jpeg2000.c\
./g2clib-1.0.4/enc_png.c\
./g2clib-1.0.4/g2_addfield.c\
./g2clib-1.0.4/g2_addgrid.c\
./g2clib-1.0.4/g2_addlocal.c\
./g2clib-1.0.4/g2_create.c\
./g2clib-1.0.4/g2_free.c\
./g2clib-1.0.4/g2_getfld.c\
./g2clib-1.0.4/g2_gribend.c\
./g2clib-1.0.4/g2_info.c\
./g2clib-1.0.4/g2_miss.c\
./g2clib-1.0.4/g2_unpack1.c\
./g2clib-1.0.4/g2_unpack2.c\
./g2clib-1.0.4/g2_unpack3.c\
./g2clib-1.0.4/g2_unpack4.c\
./g2clib-1.0.4/g2_unpack5.c\
./g2clib-1.0.4/g2_unpack6.c\
./g2clib-1.0.4/g2_unpack7.c\
./g2clib-1.0.4/gbits.c\
./g2clib-1.0.4/getdim.c\
./g2clib-1.0.4/getpoly.c\
./g2clib-1.0.4/gridtemplates.c\
./g2clib-1.0.4/int_power.c\
./g2clib-1.0.4/jpcpack.c\
./g2clib-1.0.4/jpcunpack.c\
./g2clib-1.0.4/misspack.c\
./g2clib-1.0.4/mkieee.c\
./g2clib-1.0.4/pack_gp.c\
./g2clib-1.0.4/pdstemplates.c\
./g2clib-1.0.4/pngpack.c\
./g2clib-1.0.4/pngunpack.c\
./g2clib-1.0.4/rdieee.c\
./g2clib-1.0.4/reduce.c\
./g2clib-1.0.4/seekgb.c\
./g2clib-1.0.4/simpack.c\
./g2clib-1.0.4/simunpack.c\
./g2clib-1.0.4/specpack.c\
./g2clib-1.0.4/specunpack.c\
./g2clib-1.0.4/drstemplates.h\
./g2clib-1.0.4/grib2.h\
./g2clib-1.0.4/gridtemplates.h\
./g2clib-1.0.4/pdstemplates.h\
grib2_import.cpp\
MLB_Interface.cpp\
grib2_import.h\
MLB_Interface.h

libio_grid_grib2_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
