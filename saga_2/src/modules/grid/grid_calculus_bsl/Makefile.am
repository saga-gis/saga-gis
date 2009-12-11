#
# $Id: Makefile.am,v 1.1 2009-12-11 14:09:00 oconrad Exp $
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
pkglib_LTLIBRARIES = libgrid_calculus_bsl.la
libgrid_calculus_bsl_la_SOURCES =\
ausdruck.cpp\
auswert_anweisung.cpp\
auswert_foreach.cpp\
auswert_if.cpp\
auswert_zuweisung.cpp\
basistypen.cpp\
bedingung.cpp\
diverses.cpp\
foreach.cpp\
funktion.cpp\
grid_bsl.cpp\
interpolation.cpp\
bsl_interpreter.cpp\
MLB_Interface.cpp\
pars_all.cpp\
vector.cpp\
zuweisung.cpp\
ausdruck.h\
auswert_anweisung.h\
auswert_foreach.h\
auswert_if.h\
auswert_zuweisung.h\
basistypen.h\
bedingung.h\
diverses.h\
foreach.h\
funktion.h\
funktion_numerisch.h\
funktion_statistisch.h\
grid_bsl.h\
gr_def.h\
gr_syst.h\
interpolation.h\
bsl_interpreter.h\
MLB_Interface.h\
pars_all.h\
vector.h\
zuweisung.h

libgrid_calculus_bsl_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

