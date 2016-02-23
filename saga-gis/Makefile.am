SUBDIRS = . src
if DEBUG
AM_CFLAGS = -g -DDEBUG
AM_CXXFLAGS = -g -DDEBUG $(GOMPFLAGS)
endif
ACLOCAL_AMFLAGS = -I m4


# additional directories to package
EXTRA_DIST = ./src/scripting

toolchainsdir = $(pkgdatadir)/toolchains

dist_toolchains_DATA = src/scripting/toolchains/*.xml
