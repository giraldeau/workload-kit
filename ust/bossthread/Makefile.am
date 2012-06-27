bin_PROGRAMS = wk-bossthread

AM_CFLAGS = -I $(top_srcdir)/src

wk_bossthread_SOURCES = bossthread.c bossthread_tp.c bossthread_tp_provider.h
wk_bossthread_LDADD = ../../src/libwk.a $(UST_LIBS)
wk_bossthread_CFLAGS = $(UST_CFLAGS) $(AM_CFLAGS)

