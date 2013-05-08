AC_INIT([workload-kit], 0.1.11)
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

AC_ARG_ENABLE([ust],
  [AS_HELP_STRING([--disable-ust],
    [disable ust])],
  [ust_support=$enableval], [ust_support=yes])

AS_IF([test "x$ust_support" == "xyes"], [
  PKG_CHECK_MODULES([UST], [lttng-ust], [],
	[AC_MSG_ERROR([Cannot find lttng ust library with pkg-config])])
])

AM_CONDITIONAL([HAVE_UST], [test "x$ust_support" == "xyes"])

AC_ARG_ENABLE([cputemp],
  [AS_HELP_STRING([--disable-cputemp],
    [disable cputemp])],
  [cputemp_support=$enableval], [cputemp_support=yes])

AS_IF([test "x$cputemp_support" == "xyes"], [
  AC_CHECK_HEADER(sensors/sensors.h, [],
    [AC_MSG_ERROR([can not find sensors/sensors.h])]),
  AC_CHECK_LIB(sensors, sensors_get_detected_chips,
    [SENSORS_LIBS="-lsensors"],
    [AC_MSG_ERROR([can not find -lsensors])]),
  AC_SUBST(SENSORS_LIBS)
])

AM_CONDITIONAL([HAVE_UST], [test "x$ust_support" == "xyes"])
AM_CONDITIONAL([HAVE_CPUTEMP], [test "x$cputemp_support" == "xyes"])

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
    tests/Makefile
    doc/man/Makefile
])

AS_IF([test "x$ust_support" == "xyes"],
  [AC_CONFIG_FILES([
    ust/bossthread/Makefile
    ust/functrace/Makefile
    ust/heartbeat/Makefile
    ust/hello-openmp/Makefile
    ust/threadtree/Makefile
])])

AS_IF([test "x$ust_support" == "xyes" -a "x$cputemp_support" == "xyes"],
  [AC_CONFIG_FILES([
    ust/cputemp/Makefile
])])

AC_OUTPUT
