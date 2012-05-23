bin_PROGRAMS = wk-sample wk-bossthread wk-functrace

AM_CFLAGS = -I ../src

wk_sample_SOURCES = sample.c sample_tp.c sample_tp_provider.h
wk_sample_LDADD = $(UST_LIBS)
wk_sample_CFLAGS = $(UST_CFLAGS)

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
