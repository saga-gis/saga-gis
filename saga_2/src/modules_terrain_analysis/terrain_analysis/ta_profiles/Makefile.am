#
# $Id: Makefile.am,v 1.9 2008-04-16 16:21:03 oconrad Exp $
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
pkglib_LTLIBRARIES = libta_profiles.la
libta_profiles_la_SOURCES =\
Grid_Cross_Profiles.cpp\
Grid_CrossSections.cpp\
Grid_Flow_Profile.cpp\
Grid_Profile.cpp\
Grid_ProfileFromPoints.cpp\
Grid_Profile_From_Lines.cpp\
Grid_Swath_Profile.cpp\
Polygon_Clipper.cpp\
MLB_Interface.cpp\
PDFDocEngine_CrossSections.cpp\
Geometry.h\
Grid_Cross_Profiles.h\
Grid_CrossSections.h\
Grid_Flow_Profile.h\
Grid_Profile.h\
Grid_ProfileFromPoints.h\
Grid_Profile_From_Lines.h\
Grid_Swath_Profile.h\
Polygon_Clipper.h\
MLB_Interface.h\
PDFDocEngine_CrossSections.h

libta_profiles_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

