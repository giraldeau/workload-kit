bin_PROGRAMS = ioburst bossthread deadlock dthread inception srvhog clihog lockfight functrace
ioburst_SOURCES = ioburst.c
bossthread_SOURCES = bossthread.c calibrate.c
bossthread_LDFLAGS = $(UST_LIBS)
deadlock_SOURCES = deadlock.c
dthread_SOURCES = dthread.c
inception_SOURCES = inception.c
srvhog_SOURCES = srvhog.c calibrate.h calibrate.c
clihog_SOURCES = clihog.c
lockfight_SOURCES = lockfight.c

noinst_LTLIBRARIES = libcyg.la

libcyg_la_SOURCES = cyg.c

functrace_SOURCES = functrace.c
functrace_CFLAGS = -finstrument-functions
functrace_LDFLAGS = -rdynamic $(UST_LIBS) libcyg.la
