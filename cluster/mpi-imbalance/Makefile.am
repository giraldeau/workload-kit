bin_PROGRAMS = mpi-imbalance
AM_CFLAGS = -I $(top_srcdir)/src
mpi_imbalance_SOURCES = ust_tracepoint.c ust_tracepoint.h mpi-imbalance.c
mpi_imbalance_LDADD = $(top_srcdir)/src/libwk.a $(UST_LIBS)
mpi_imbalance_CFLAGS = $(UST_CFLAGS) $(AM_CFLAGS)
