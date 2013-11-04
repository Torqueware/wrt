# autoscan + editing by William Millard~!       -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Note, the ordering of this configuration is intentional! Do NOT change the
# ordering!

# Sanity Preserving Section Order:
#	1.  Autoconf requirements
#	2.  Package information
#	   'AC_INIT(PACKAGE, VERSION, EMAIL)'
#	3.  Libraries
#	4.  Header files
#	5.  Types
#	6.  Structures
#	7.  Compiler characteristics
#	8.  Library functions
#	9.  System services
#	10. Config files
#	   'AC_CONFIG_FILES([FILE...])'
#	11. Configuration output
#	   'AC_OUTPUT'

###############################################################################
# BELOW BE DRAGONS                                                            #
###############################################################################

# Autoconf requirements
AC_PREREQ([2.69])

# Package information
AC_INIT([WRT Remote Toolkit],[0.1],[wmillard1@gmail.com],[wrt])
AC_CONFIG_SRCDIR([src/main.cxx])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([src/include/config.h])

# Start automake
AM_INIT_AUTOMAKE([1.9 no-define -Wall -Werror foreign subdir-objects])
AM_PROG_AR

# Start libtool
LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_YACC
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LEX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
AC_CHECK_LIB([config], [-lconfig++])
AC_CHECK_LIB([c1], [-lc1]) #SILLY
AC_CHECK_LIB([p2], [-lp2]) #SILLY, TOO

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([fcntl.h inttypes.h libintl.h limits.h locale.h malloc.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/time.h syslog.h unistd.h])

# Checks for types / compiler characteristics
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([string iostream cstdint gettimeofday memmove memset mkdir munmap regcomp setlocale strcasecmp strchr strcspn strdup strncasecmp strpbrk strrchr strspn strtol strtoul uname])

# Checks for system services

# Makefiles
AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 man/Makefile
                 scripts/Makefile
                 scripts/cert/Makefile
                 scripts/config/Makefile
                 scripts/radio/Makefile
                 scripts/wrt-image/Makefile
                 src/Makefile
                 src/include/Makefile
                 src/lib/Makefile
                 src/lib/ssh/Makefile
                 src/lib/wrt/Makefile
                 test/Makefile])

# DIRTY DIRTY OVERRIDE TO FORCE C++0x
CXXFLAGS="$CXXFLAGS -std=c++0x"

# Generate the configuration
AC_OUTPUT
