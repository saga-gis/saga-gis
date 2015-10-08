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
pkglib_LTLIBRARIES = libimagery_classification.la
libimagery_classification_la_SOURCES =\
change_detection.cpp\
classification_quality.cpp\
classify_cluster_analysis.cpp\
classify_supervised.cpp\
classify_supervised_polygons.cpp\
decision_tree.cpp\
MLB_Interface.cpp\
change_detection.h\
classification_quality.h\
classify_cluster_analysis.h\
classify_supervised.h\
classify_supervised_polygons.h\
decision_tree.h\
MLB_Interface.h

libimagery_classification_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

