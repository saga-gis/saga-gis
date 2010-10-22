#
# $Id: Makefile.am,v 1.1 2010-10-22 11:59:52 oconrad Exp $
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
pkglib_LTLIBRARIES = libdocs_html.la
libdocs_html_la_SOURCES =\
CreateWebContent.cpp\
doc_html.cpp\
doc_svg.cpp\
MLB_Interface.cpp\
CreateWebContent.cpp\
doc_html.cpp\
doc_svg.cpp\
MLB_Interface.cpp

libdocs_html_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

