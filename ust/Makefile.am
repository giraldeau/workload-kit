bin_PROGRAMS = sample bossthread

AM_CFLAGS = -I ../src

sample_SOURCES = sample.c sample_tp.c
sample_LDFLAGS = $(UST_LIBS)

bossthread_SOURCES = bossthread.c bossthread_tp.c
bossthread_LDFLAGS = ../src/libwk.a $(UST_LIBS)