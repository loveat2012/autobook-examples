## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2000 Gary V. Vaughan
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

MAINTAINERCLEANFILES	= Makefile.in common.h stamp-common
EXTRA_DIST		= common-h.in

AM_CPPFLAGS		= -DMODULE_PATH=\"$(libdir)/@PACKAGE@\"
INCLUDES		= -I$(top_builddir) -I$(top_srcdir)

pkginclude_HEADERS 	= builtin.h common.h error.h eval.h list.h \
			module.h sic.h syntax.h
noinst_HEADERS 	= ltdl.h

## @start 1
lib_LTLIBRARIES 	= libcommon.la libsic.la

libsic_la_LIBADD	= $(top_builddir)/replace/libreplace.la \
			libcommon.la @LIBADD_DL@
libsic_la_SOURCES 	= builtin.c error.c eval.c list.c ltdl.c \
			module.c sic.c syntax.c
## @end 1

libcommon_la_SOURCES	= xmalloc.c xstrdup.c xstrerror.c

# Regenerate common.h with config.status whenever common-h.in changes.
common.h: stamp-common
	@:
stamp-common: $(srcdir)/common-h.in $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES= CONFIG_HEADERS= CONFIG_OTHER=sic/common.h $(SHELL) ./config.status
	echo timestamp > $@

# Don't distribute common.h, since it is build host dependent!
dist-hook:
	rm -f $(distdir)/common.h
