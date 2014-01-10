#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
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
pkglib_LTLIBRARIES = libgarden_learn_to_program.la
libgarden_learn_to_program_la_SOURCES =\
Exercise_01.cpp\
Exercise_02.cpp\
Exercise_03.cpp\
Exercise_04.cpp\
Exercise_05.cpp\
Exercise_06.cpp\
Exercise_07.cpp\
Exercise_08.cpp\
Exercise_09.cpp\
Exercise_10.cpp\
Exercise_11.cpp\
Exercise_12.cpp\
Exercise_13.cpp\
Exercise_14.cpp\
MLB_Interface.cpp\
Exercise_01.h\
Exercise_02.h\
Exercise_03.h\
Exercise_04.h\
Exercise_05.h\
Exercise_06.h\
Exercise_07.h\
Exercise_08.h\
Exercise_09.h\
Exercise_10.h\
Exercise_11.h\
Exercise_12.h\
Exercise_13.h\
Exercise_14.h\
MLB_Interface.h

libgarden_learn_to_program_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

