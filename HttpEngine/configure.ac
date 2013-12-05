#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(libHttpEngine, 1.00, libo.weng@gmail.com)
#AC_CONFIG_SRCDIR([config.h])
#AC_CONFIG_HEADER([config.h])

AC_SUBST([PLATFORM_NAME])
PLATFORM_NAME=$(uname)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AM_INIT_AUTOMAKE
AC_PROG_RANLIB
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_VOLATILE

# Checks for library functions.
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([select])

AC_OUTPUT([Makefile])
