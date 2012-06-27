AC_INIT([workload-kit], 0.1.4)
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign dist-bzip2])
AC_CONFIG_MACRO_DIR([config])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_INIT

AC_CHECK_HEADERS([ \
	sys/types.h unistd.h fcntl.h strings.h pthread.h time.h \
	errno.h stdarg.h limits.h signal.h stdlib.h
])

AC_CHECK_LIB(pthread, pthread_create)

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([UST], [lttng-ust], [have_ust="yes"], [have_ust="no"])
AS_IF([test "x${have_ust}" == "xno"], AC_MSG_ERROR([lttng-ust is required and not found]))

AC_OPENMP

# be silent by default
AM_SILENT_RULES([yes])

AC_PROG_LIBTOOL
AC_PROG_CC
AM_PROG_CC_C_O
AC_CONFIG_FILES([Makefile
    utils/Makefile
    src/Makefile
    doc/Makefile
    ust/Makefile
    ust/bossthread/Makefile
    ust/functrace/Makefile
    ust/heartbeat/Makefile
    ust/hello-openmp/Makefile
    ust/threadtree/Makefile
    doc/man/Makefile
])
AC_OUTPUT
