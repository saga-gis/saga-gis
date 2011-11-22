#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
if HAVE_HARU 
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS) $(DISABLELIBHARU)
AM_LDFLAGS         = -fPIC -shared -avoid-version $(HARU_LIB)
pkglib_LTLIBRARIES = libdocs_pdf.la
libdocs_pdf_la_SOURCES =\
doc_pdf.cpp\
MLB_Interface.cpp\
Profile_Cross_Sections.cpp\
Shapes_Report.cpp\
Shapes_Summary.cpp\
doc_pdf.h\
MLB_Interface.h\
Profile_Cross_Sections.h\
Shapes_Report.h\
Shapes_Summary.h

libdocs_pdf_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
endif
