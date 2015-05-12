#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(test_webrtc, 0.1, sunkwei@gmail.com)
AC_CONFIG_SRCDIR([webrtc_apm.cpp])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
PKG_CHECK_MODULES([WEBRTC_AP], webrtc-audio-processing)
PKG_CHECK_MODULES([SNDFILE], sndfile)

# Checks for header files.
AC_CHECK_HEADERS([memory.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
