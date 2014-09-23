dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)

AC_INIT([GSquares], [0.1])
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR(src/initializer.c)
AC_CONFIG_HEADERS(config.h)
AM_SANITY_CHECK

AM_MAINTAINER_MODE

AC_PROG_CC
AC_ISC_POSIX
AC_HEADER_STDC

AC_CHECK_LIB(m, sqrt)
AC_CHECK_LIB(glib-2.0, g_timeout_add)
AC_CHECK_LIB(gtk-x11-2.0, gtk_main)
AC_CHECK_LIB(cairo, cairo_stroke)

CFLAGS="-O3 -std=gnu99 -pipe -pthread `pkg-config --cflags gtk+-2.0`"
LIBS="`pkg-config --libs gtk+-2.0` -lm"

dnl we use -Wall if we have gcc
if test "x$GCC" = "xyes"; then
  if test -z "`echo "$CFLAGS" | grep "\-Wall" 2> /dev/null`" ; then
    CFLAGS="$CFLAGS -Wall"
  fi
fi

AC_SUBST(CFLAGS)
AC_SUBST(LIBS)

AC_CHECK_HEADERS(stdlib.h string.h time.h)

AC_HEADER_STDBOOL

AC_FUNC_REALLOC
AC_CHECK_FUNCS(memmove sqrt)


AC_OUTPUT([Makefile     \
            src/Makefile  \
            src/paths.h   \
            pixmaps/Makefile])
