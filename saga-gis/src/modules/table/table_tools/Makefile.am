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
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libtable_tools.la
libtable_tools_la_SOURCES =\
Join_Tables.cpp\
MLB_Interface.cpp\
table_categories_to_indicators.cpp\
table_change_color_format.cpp\
table_change_date_format.cpp\
table_change_field_type.cpp\
Table_Create_Empty.cpp\
Table_Enumerate.cpp\
table_field_deletion.cpp\
Table_Rotate.cpp\
table_selection.cpp\
table_text_replacer.cpp\
Join_Tables.h\
MLB_Interface.h\
table_categories_to_indicators.h\
table_change_color_format.h\
table_change_date_format.h\
table_change_field_type.h\
Table_Create_Empty.h\
Table_Enumerate.h\
table_field_deletion.h\
Table_Rotate.h\
table_selection.h\
table_text_replacer.h

libtable_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

