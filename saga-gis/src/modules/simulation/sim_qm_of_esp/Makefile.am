#
# $Id: Makefile.am 1537 2012-11-19 20:05:19Z reklov_w $
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
pkglib_LTLIBRARIES = libsim_qm_of_esp.la
libsim_qm_of_esp_la_SOURCES =\
fill_sinks.cpp\
flow_routing.cpp\
hillslope_evolution_adi.cpp\
hillslope_evolution_ftcs.cpp\
MLB_Interface.cpp\
successive_flow_routing.cpp\
fill_sinks.h\
flow_routing.h\
hillslope_evolution_adi.h\
hillslope_evolution_ftcs.h\
MLB_Interface.h\
successive_flow_routing.h

libsim_qm_of_esp_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

