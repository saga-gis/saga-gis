#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libta_profiles.la
libta_profiles_la_SOURCES =\
Grid_Cross_Profiles.cpp\
Grid_Flow_Profile.cpp\
Grid_Profile.cpp\
Grid_ProfileFromPoints.cpp\
Grid_Profile_From_Lines.cpp\
Grid_Swath_Profile.cpp\
MLB_Interface.cpp\
Grid_Cross_Profiles.h\
Grid_Flow_Profile.h\
Grid_Profile.h\
Grid_ProfileFromPoints.h\
Grid_Profile_From_Lines.h\
Grid_Swath_Profile.h\
MLB_Interface.h

libta_profiles_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

