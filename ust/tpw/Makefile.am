bin_PROGRAMS = wk-tpw

AM_CFLAGS = -I $(top_srcdir)/src

wk_tpw_SOURCES = tpw-main.c
wk_tpw_LDADD = libtpw.la

include_HEADERS = tpw.h
lib_LTLIBRARIES = libtpw.la
libtpw_la_SOURCES = tpw.c tpw_tp.c tpw_tp_provider.h
libtpw_la_LIBADD = $(UST_LIBS)
libtpw_la_CFLAGS = $(UST_CFLAGS)

