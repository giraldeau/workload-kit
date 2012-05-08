AC_INIT([Workload kit], 0.1)
AM_INIT_AUTOMAKE

LT_INIT

AC_CHECK_HEADERS(sys/types.h unistd.h fcntl.h strings.h pthread.h time.h errno.h stdarg.h limits.h signal.h stdlib.h)
AC_CHECK_LIB(pthread, pthread_create)

AC_ARG_ENABLE(ust,
        AC_HELP_STRING(--enable-ust , Build with support for Userspace Tracer.),
        [ ENABLE_UST="$enable_ust" ],
        [ ENABLE_UST="no" ]
)
UST_LIBS=""
WITH_UST=""
if test "$ENABLE_UST" = "yes"; then
	AC_CHECK_LIB(ust, trace_mark)
	UST_LIBS="-llttng-ust -ldl"
	AC_DEFINE(WITH_UST, 1, "Defined to 1 if UST support is enabled")
fi
AC_SUBST(UST_LIBS)
AC_SUBST(WITH_UST)
AM_CONDITIONAL([WITH_UST], [ test "$ENABLE_UST" = "yes" ])

# be silent by default
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_CONFIG_FILES([Makefile
    src/Makefile
    ust/Makefile])
AC_OUTPUT
