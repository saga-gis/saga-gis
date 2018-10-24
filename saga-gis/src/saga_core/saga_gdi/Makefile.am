if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEP_DEFS = -fPIC `wx-config --unicode=yes --static=no --cxxflags`
DEP_LFLG = -fPIC -shared `wx-config --unicode=yes --static=no --libs`

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = $(DEP_LFLG)
lib_LTLIBRARIES    = libsaga_gdi.la
libsaga_gdi_la_LDFLAGS=$(AM_LDFLAGS) -release $(VERSION)
libsaga_gdi_la_SOURCES =\
3d_view_canvas.cpp\
3d_view_dialog.cpp\
3d_view_panel.cpp\
3d_view_projector.cpp\
sgdi_diagram.cpp\
sgdi_dialog.cpp\
sgdi_helper.cpp\
saga_gdi.cpp\
3d_view.h\
3d_view_tools.h\
sgdi_core.h\
sgdi_diagram.h\
sgdi_dialog.h\
sgdi_helper.h\
saga_gdi.h

otherincludedir = $(pkgincludedir)/saga_core/saga_gdi
otherinclude_HEADERS = saga_gdi.h \
3d_view.h\
3d_view_tools.h\
sgdi_core.h\
sgdi_diagram.h\
sgdi_dialog.h\
sgdi_helper.h

libsaga_gdi_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
