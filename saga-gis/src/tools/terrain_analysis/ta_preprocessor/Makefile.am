#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libta_preprocessor.la
libta_preprocessor_la_SOURCES =\
breach_depressions.cpp\
burn_in_streams.cpp\
FillSinks.cpp\
FillSinks_WL.cpp\
FillSinks_WL_XXL.cpp\
MLB_Interface.cpp\
Flat_Detection.cpp\
Pit_Eliminator.cpp\
Pit_Router.cpp\
breach_depressions.h\
burn_in_streams.h\
FillSinks.h\
FillSinks_WL.h\
Flat_Detection.h\
Pit_Eliminator.h\
Pit_Router.h

libta_preprocessor_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

