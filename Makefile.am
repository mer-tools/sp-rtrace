include_HEADERS = src/library/sp_rtrace_context.h src/library/sp_rtrace_formatter.h src/library/sp_rtrace_defs.h \
	src/library/sp_rtrace_tracker.h src/library/sp_rtrace_parser.h src/library/sp_rtrace_filter.h

dist_bin_SCRIPTS = scripts/rtrace-* devscripts/rtrace-module-gen

SUBDIRS = src doc tests

DISTCLEANFILES = Makefile Makefile.in configure config.* autoscan.log aclocal.m4 config-*

distclean-local:
	rm -rf aux m4 autom4te.cache sp-rtrace.log sp-rtrace.sum
