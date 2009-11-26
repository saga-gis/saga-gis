if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
DEP_DEFS = -fPIC `wx-config --unicode=yes --static=no --cxxflags` -D_SAGA_UNICODE $(DBGFLAGS)
DEP_LFLG = -fPIC -shared `wx-config --unicode=yes --static=no --libs`
else
DEP_DEFS = -fPIC `wx-config --unicode=no --static=no --cxxflags` $(DBGFLAGS)
DEP_LFLG = -fPIC -shared `wx-config --unicode=no --static=no --libs`
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = $(DEP_LFLG)
lib_LTLIBRARIES    = libsaga_gdi.la
libsaga_gdi_la_SOURCES =\
sgdi_diagram.cpp\
sgdi_dialog.cpp\
sgdi_helper.cpp\
saga_gdi.cpp\
sgdi_diagram.h\
sgdi_dialog.h\
sgdi_helper.h\
saga_gdi.h

libsaga_gdi_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
