#
# $Id: Makefile.am,v 1.8 2010-11-05 14:51:46 oconrad Exp $
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
pkglib_LTLIBRARIES = libtable_calculus.la
libtable_calculus_la_SOURCES =\
Fit.cpp\
LMFit.cpp\
MLB_Interface.cpp\
Table_Calculator.cpp\
Table_CalculatorShapes.cpp\
table_cluster_analysis.cpp\
table_running_average.cpp\
Table_Trend.cpp\
Fit.h\
LMFit.h\
MLB_Interface.h\
Table_Calculator.h\
Table_CalculatorShapes.h\
table_cluster_analysis.h\
table_running_average.h\
Table_Trend.h

libtable_calculus_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

