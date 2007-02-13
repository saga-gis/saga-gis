#
# $Id: Makefile.am,v 1.5 2007-02-13 12:49:19 tschorr Exp $
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
pkglib_LTLIBRARIES = libgrid_visualisation.la
libgrid_visualisation_la_SOURCES =\
Grid_3D_Image.cpp\
Grid_Color_Blend.cpp\
Grid_Color_Rotate.cpp\
Grid_Colors_Fit.cpp\
Grid_RGB_Composite.cpp\
MLB_Interface.cpp\
Grid_3D_Image.h\
Grid_Color_Blend.h\
Grid_Color_Rotate.h\
Grid_Colors_Fit.h\
Grid_RGB_Composite.h\
MLB_Interface.h

libgrid_visualisation_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

