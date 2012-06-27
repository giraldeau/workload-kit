bin_PROGRAMS = wk-functrace

AM_CFLAGS = -I $(top_srcdir)/src

noinst_LTLIBRARIES = libcyg.la
libcyg_la_SOURCES = cyg.c cyg_tp.c cyg_tp_provider.h
libcyg_la_LIBADD = $(UST_LIBS)
libcyg_la_CFLAGS = $(UST_CFLAGS)

wk_functrace_SOURCES = functrace.c
wk_functrace_CFLAGS = -finstrument-functions
wk_functrace_LDFLAGS = -rdynamic
wk_functrace_LDADD = libcyg.la $(UST_LIBS)

