bin_PROGRAMS = wk-cputemp

AM_CFLAGS = -I $(top_srcdir)/src

wk_cputemp_SOURCES = cputemp.c cputemp.h cputemp_tp.c cputemp_tp_provider.h
wk_cputemp_LDADD = $(UST_LIBS)
wk_cputemp_CFLAGS = $(UST_CFLAGS)
wk_cputemp_LDFLAGS = $(SENSORS_LIBS)

