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
pkglib_LTLIBRARIES = libimagery_tools.la
libimagery_tools_la_SOURCES =\
evi.cpp\
Image_VI_Distance.cpp\
Image_VI_Slope.cpp\
MLB_Interface.cpp\
pansharpening.cpp\
tasseled_cap.cpp\
evi.h\
Image_VI_Distance.h\
Image_VI_Slope.h\
MLB_Interface.h\
pansharpening.h\
tasseled_cap.h

libimagery_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

