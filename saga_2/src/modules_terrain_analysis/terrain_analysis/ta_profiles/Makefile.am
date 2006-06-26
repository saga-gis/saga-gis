DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libta_profiles.la
libta_profiles_la_SOURCES =\
Grid_CrossSections.cpp\
Grid_Flow_Profile.cpp\
Grid_Profile.cpp\
Grid_ProfileFromPoints.cpp\
Grid_Swath_Profile.cpp\
Intersection_GPC.c\
MLB_Interface.cpp\
PDFDocEngine_CrossSections.cpp\
Grid_CrossSections.h\
Grid_Flow_Profile.h\
Grid_Profile.h\
Grid_ProfileFromPoints.h\
Grid_Swath_Profile.h\
Intersection_GPC.h\
MLB_Interface.h\
PDFDocEngine_CrossSections.h

libta_profiles_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libta_profiles.so ta_profiles.so; popd
