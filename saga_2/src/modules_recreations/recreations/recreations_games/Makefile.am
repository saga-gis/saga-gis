if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = librecreations_games.la
librecreations_games_la_SOURCES =\
Mine_Sweeper.cpp\
Mine_Sweeper_Res.cpp\
MLB_Interface.cpp\
Sudoku.cpp\
Mine_Sweeper.h\
MLB_Interface.h\
Sudoku.h

librecreations_games_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s librecreations_games.so recreations_games.so; popd
