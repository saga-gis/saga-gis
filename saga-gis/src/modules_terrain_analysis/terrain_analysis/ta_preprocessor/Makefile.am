#
# $Id$
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
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libta_preprocessor.la
libta_preprocessor_la_SOURCES =\
FillSinks.cpp\
FillSinks_WL.cpp\
FillSinks_WL_XXL.cpp\
MLB_Interface.cpp\
Flat_Detection.cpp\
Pit_Eliminator.cpp\
Pit_Router.cpp\
FillSinks.h\
FillSinks_WL.h\
MLB_Interface.h\
Flat_Detection.h\
Pit_Eliminator.h\
Pit_Router.h

libta_preprocessor_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

