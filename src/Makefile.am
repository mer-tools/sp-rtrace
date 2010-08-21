
lib_LTLIBRARIES = libsp-rtrace1.la libsp-rtrace-main.la \
	libsp-rtrace-memory.la libsp-rtrace-memtransfer.la \
	libsp-rtrace-shmaddr.la libsp-rtrace-shmseg.la

libsp_rtrace1_la_SOURCES = library/sp_rtrace_context.c library/sp_rtrace_formatter.c \
	common/formatter.c common/dlist.c common/htable.c common/utils.c common/rtrace_data.c
libsp_rtrace1_la_LDFLAGS = -version-info $(VERSION_MAJOR):$(VERSION_MINOR)
libsp_rtrace1_la_LIBADD = -ldl

libsp_rtrace_main_la_SOURCES = modules/sp_rtrace_main.c rtrace/rtrace_env.c modules/sp_context_impl.c common/utils.c
libsp_rtrace_main_la_LDFLAGS = -version-info $(VERSION_MAJOR):$(VERSION_MINOR)
libsp_rtarce_main_la_CGLAGS = -rdynamic
libsp_rtrace_main_la_LIBADD = -lrt -ldl -lpthread

libsp_rtrace_memory_la_SOURCES = modules/sp_rtrace_memory.c
libsp_rtarce_memory_la_CGLAGS = -rdynamic
libsp_rtrace_memory_la_LDFLAGS = -version-info $(VERSION_MAJOR):$(VERSION_MINOR)
libsp_rtrace_memory_la_LIBADD = -ldl -lpthread 

libsp_rtrace_memtransfer_la_SOURCES = modules/sp_rtrace_memtransfer.c
libsp_rtarce_memtransfer_la_CGLAGS = -rdynamic
libsp_rtrace_memtransfer_la_LDFLAGS = -version-info $(VERSION_MAJOR):$(VERSION_MINOR)
libsp_rtrace_memtransfer_la_LIBADD = -ldl 

libsp_rtrace_shmaddr_la_SOURCES = modules/sp_rtrace_shmaddr.c
libsp_rtarce_shmaddr_la_CGLAGS = -rdynamic
libsp_rtrace_shmaddr_la_LDFLAGS = -version-info $(VERSION_MAJOR):$(VERSION_MINOR)
libsp_rtrace_shmaddr_la_LIBADD = -ldl 

libsp_rtrace_shmseg_la_SOURCES = modules/sp_rtrace_shmseg.c common/htable.c common/dlist.c
libsp_rtarce_shmseg_la_CGLAGS = -rdynamic
libsp_rtrace_shmseg_la_LDFLAGS = -version-info $(VERSION_MAJOR):$(VERSION_MINOR)
libsp_rtrace_shmseg_la_LIBADD = -ldl 

bin_PROGRAMS = sp-rtrace sp-rtrace-postproc sp-rtrace-resolve sp-rtrace-allocmap

sp_rtrace_SOURCES = rtrace/sp_rtrace.c rtrace/listener.c rtrace/rtrace_env.c common/utils.c \
	common/dlist.c common/rtrace_data.c common/htable.c 
sp_rtrace_CFLAGS = $(AM_CFLAGS)
sp_rtrace_LDFLAGS = -Wl,-z,defs
sp_rtrace_LDADD = -ldl

sp_rtrace_postproc_SOURCES = rtrace-postproc/sp_rtrace_postproc.c rtrace-postproc/parse_binary.c \
    rtrace-postproc/parse_text.c rtrace-postproc/leaks_sort.c rtrace-postproc/writer.c rtrace-postproc/filter.c \
    common/rtrace_data.c common/htable.c common/dlist.c common/utils.c common/formatter.c
sp_rtrace_postproc_CFLAGS = $(AM_CFLAGS)
sp_rtrace_postproc_LDFLAGS = -Wl,-z,defs

sp_rtrace_resolve_SOURCES = rtrace-resolve/sp_rtrace_resolve.c common/utils.c common/rtrace_data.c common/dlist.c \
	rtrace-resolve/sarray.c common/htable.c rtrace-resolve/namecache.c rtrace-resolve/resolver.c 
sp_rtrace_resolve_CFLAGS = $(AM_CFLAGS)
sp_rtrace_resolve_LDFLAGS = -Wl,-z,defs
sp_rtrace_resolve_LDADD = -lbfd

sp_rtrace_allocmap_SOURCES = rtrace-allocmap/sp_rtrace_allocmap.c
sp_rtrace_allocmap_CFLAGS = $(AM_CFLAGS)
sp_rtrace_allocmap_LDFLAGS = -Wl,-z,defs

DISTCLEANFILES = Makefile.in *la