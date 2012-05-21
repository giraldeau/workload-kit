bin_PROGRAMS = sample bossthread functrace

AM_CFLAGS = -I ../src

sample_SOURCES = sample.c sample_tp.c
sample_LDFLAGS = $(UST_LIBS)

bossthread_SOURCES = bossthread.c bossthread_tp.c
bossthread_LDFLAGS = ../src/libwk.a $(UST_LIBS)

noinst_LTLIBRARIES = libcyg.la

libcyg_la_SOURCES = cyg.c cyg_tp.c
libcyg_la_LDFLAGS = $(UST_LIBS)

functrace_SOURCES = functrace.c
functrace_CFLAGS = -finstrument-functions
functrace_LDFLAGS = -rdynamic libcyg.la
