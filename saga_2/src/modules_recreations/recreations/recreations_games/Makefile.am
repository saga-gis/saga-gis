#
# $Id: Makefile.am,v 1.4 2007-02-13 09:58:02 tschorr Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared
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

