SUBDIRS = . src
if DEBUG
AM_CFLAGS = -g -DDEBUG
AM_CXXFLAGS = -g -DDEBUG $(GOMPFLAGS)
endif
ACLOCAL_AMFLAGS = -I m4

# additional directories to package
EXTRA_DIST = ./src/scripting
dist-hook:
		rm -rf `find $(distdir)/src/scripting -type d -name .svn`
