if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libtable_calculus.la
libtable_calculus_la_SOURCES =\
Fit.cpp\
LMFit.cpp\
MLB_Interface.cpp\
Table_Calculator.cpp\
Table_CalculatorShapes.cpp\
Table_Trend.cpp\
Fit.h\
LMFit.h\
MLB_Interface.h\
Table_Calculator.h\
Table_CalculatorShapes.h\
Table_Trend.h

libtable_calculus_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

