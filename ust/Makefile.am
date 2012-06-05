bin_PROGRAMS = wk-heartbeat wk-bossthread wk-functrace wk-threadtree

AM_CFLAGS = -I $(top_srcdir)/src

wk_heartbeat_SOURCES = heartbeat.c heartbeat_tp.c heartbeat_tp_provider.h
wk_heartbeat_LDADD = $(UST_LIBS)
wk_heartbeat_CFLAGS = $(UST_CFLAGS)

wk_bossthread_SOURCES = bossthread.c bossthread_tp.c bossthread_tp_provider.h
wk_bossthread_LDADD = ../src/libwk.a $(UST_LIBS)
wk_bossthread_CFLAGS = $(UST_CFLAGS) $(AM_CFLAGS)

noinst_LTLIBRARIES = libcyg.la
libcyg_la_SOURCES = cyg.c cyg_tp.c cyg_tp_provider.h
libcyg_la_LIBADD = $(UST_LIBS)
libcyg_la_CFLAGS = $(UST_CFLAGS)

wk_functrace_SOURCES = functrace.c
wk_functrace_CFLAGS = -finstrument-functions
wk_functrace_LDFLAGS = -rdynamic
wk_functrace_LDADD = libcyg.la $(UST_LIBS)

wk_threadtree_SOURCES = threadtree.c
wk_threadtree_LDADD = $(UST_LIBS)
wk_threadtree_CFLAGS = $(UST_CFLAGS) $(OPENMP_CFLAGS)
