bin_PROGRAMS = ioburst bossthread deadlock dthread inception srvhog clihog lockfight functrace-ust functrace-plain cpm1 cpm2
ioburst_SOURCES = ioburst.c
bossthread_SOURCES = bossthread.c
bossthread_LDFLAGS = $(UST_LIBS)
bossthread_LDADD = libwk.a

deadlock_SOURCES = deadlock.c

dthread_SOURCES = dthread.c

inception_SOURCES = inception.c
srvhog_SOURCES = srvhog.c
srvhog_LDADD = libwk.a

clihog_SOURCES = clihog.c
lockfight_SOURCES = lockfight.c

noinst_LTLIBRARIES = libcyg.la

libcyg_la_SOURCES = cyg.c
libcyg_la_LDFLAGS = $(UST_LIBS)

functrace_ust_SOURCES = functrace.c
functrace_ust_CFLAGS = -finstrument-functions
functrace_ust_LDFLAGS = -rdynamic libcyg.la

functrace_plain_SOURCES = functrace.c

cpm1_SOURCES = cpm1.c
cpm1_LDADD = libwk.a

cpm2_SOURCES = cpm2.c
cpm2_LDADD = libwk.a

noinst_LIBRARIES = libwk.a

libwk_a_SOURCES = calibrate.h calibrate.c
