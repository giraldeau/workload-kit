AC_INIT([Workload kit], 0.1)
AM_INIT_AUTOMAKE

#### ALSA support (optional) ####

ASOUNDLIB=asound
AC_ARG_ENABLE([alsa],
    AS_HELP_STRING([--disable-alsa],[Disable optional ALSA support]),
        [
            case "${enableval}" in
                yes) alsa=yes ;;
                no) alsa=no ;;
                *) AC_MSG_ERROR(bad value ${enableval} for --disable-alsa) ;;
            esac
        ],
        [alsa=auto])

if test "x${alsa}" != xno ; then
    PKG_CHECK_MODULES(ASOUNDLIB, [ alsa >= 1.0.19 ],
        [
            HAVE_ALSA=1
            AC_DEFINE([HAVE_ALSA], 1, [Have ALSA?])
        ],
        [
            HAVE_ALSA=0
            if test "x$alsa" = xyes ; then
                AC_MSG_ERROR([*** Needed alsa >= 1.0.19 support not found])
            fi
        ])
else
    HAVE_ALSA=0
fi

AC_SUBST(ASOUNDLIB_CFLAGS)
AC_SUBST(ASOUNDLIB_LIBS)
AC_SUBST(HAVE_ALSA)
AM_CONDITIONAL([HAVE_ALSA], [test "x$HAVE_ALSA" = x1])

# be silent by default
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_CONFIG_FILES([Makefile
    src/Makefile])
AC_OUTPUT
