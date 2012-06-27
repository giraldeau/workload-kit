bin_PROGRAMS = wk-threadtree

AM_CFLAGS = -I $(top_srcdir)/src

wk_threadtree_SOURCES = threadtree.c threadtree_tp.c threadtree_tp_provider.h
wk_threadtree_LDADD = $(UST_LIBS)
wk_threadtree_CFLAGS = $(UST_CFLAGS) $(OPENMP_CFLAGS)
