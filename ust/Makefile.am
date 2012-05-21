bin_PROGRAMS = wk-sample wk-bossthread wk-functrace

AM_CFLAGS = -I ../src

wk_sample_SOURCES = sample.c sample_tp.c
wk_sample_LDFLAGS = $(UST_LIBS)

wk_bossthread_SOURCES = bossthread.c bossthread_tp.c
wk_bossthread_LDFLAGS = ../src/libwk.a $(UST_LIBS)

noinst_LTLIBRARIES = libcyg.la
libcyg_la_SOURCES = cyg.c cyg_tp.c
libcyg_la_LDFLAGS = $(UST_LIBS)

wk_functrace_SOURCES = functrace.c
wk_functrace_CFLAGS = -finstrument-functions
wk_functrace_LDFLAGS = -rdynamic libcyg.la
