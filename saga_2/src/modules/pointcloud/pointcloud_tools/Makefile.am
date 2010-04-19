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
pkglib_LTLIBRARIES = libpointcloud_tools.la
libpointcloud_tools_la_SOURCES =\
MLB_Interface.cpp\
pc_cut.cpp\
pc_drop_attribute.cpp\
pc_from_grid.cpp\
pc_from_shapes.cpp\
pc_reclass_extract.cpp\
pc_thinning_simple.cpp\
pc_to_grid.cpp\
pc_to_shapes.cpp\
pc_transform.cpp\
MLB_Interface.h\
pc_cut.h\
pc_drop_attribute.h\
pc_from_grid.h\
pc_from_shapes.h\
pc_reclass_extract.h\
pc_thinning_simple.h\
pc_to_grid.h\
pc_to_shapes.h\
pc_transform.h

libpointcloud_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

