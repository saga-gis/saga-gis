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
pkglib_LTLIBRARIES = libimagery_maxent.la
libimagery_maxent_la_SOURCES =\
classify_grid.cpp\
lbfgs.cpp\
maxent.cpp\
me.cpp\
MLB_Interface.cpp\
owlqn.cpp\
presence_prediction.cpp\
sgd.cpp\
classify_grid.h\
lbfgs.h\
mathvec.h\
maxent.h\
me.h\
MLB_Interface.h\
presence_prediction.h

libimagery_maxent_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

