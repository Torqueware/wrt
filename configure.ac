# autoscan + editing by William Millard~!       -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([WRT Remote Toolkit],[0.1],[wmillard1@gmail.com],[wrt])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([src/include/config.h])

# Start the automake generation process
AM_INIT_AUTOMAKE([1.9 no-define -Wall -Werror foreign subdir-objects])

#Start libtool
LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lssh':
AC_CHECK_LIB([ssh], [config])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([string iostream cstdint atexit strtoul])

AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 man/Makefile
                 scripts/Makefile
                 scripts/cert/Makefile
                 scripts/config/Makefile
                 scripts/radio/Makefile
                 src/Makefile
                 src/include/Makefile
                 src/lib/Makefile
                 src/lib/ssh/Makefile
                 src/lib/wrt/Makefile])

CXXFLAGS="$CXXFLAGS -std=c++0x"
AC_OUTPUT
