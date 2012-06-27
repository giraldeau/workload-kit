bin_PROGRAMS = wk-heartbeat

AM_CFLAGS = -I $(top_srcdir)/src

wk_heartbeat_SOURCES = heartbeat.c heartbeat_tp.c heartbeat_tp_provider.h
wk_heartbeat_LDADD = $(UST_LIBS)
wk_heartbeat_CFLAGS = $(UST_CFLAGS)

