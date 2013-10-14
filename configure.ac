# Prelude, and project information
AC_INIT([WRT Remote Toolkit],[0.1],[wmillard1@gmail.com],[wrt])

# Config src, include, aux-build (scripts), and m4 (macros) dir
AC_CONFIG_SRCDIR([src])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([src/include/config.h])

# Start the automake generation process
AM_INIT_AUTOMAKE([1.9 no-define -Wall -Werror foreign subdir-objects])

# Check for g++ compiler and linker
AC_PROG_CXXCPP
AC_PROG_CXX
AC_PROG_CC

#Initialize libtool
LT_INIT

#Sanity checks for C and C++
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Use C++ compiler for the checks below and set default lang
AC_LANG([C++])

# Checks for specific header files
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([iostream])
AC_CHECK_HEADERS([cstdint])

# Check for other libs, headers, typedefs, structs, and compiler configs

# A listing of all makefiles in the project is REQUIRED below
AC_CONFIG_FILES([Makefile
                   src/Makefile
                     src/include/Makefile
                     src/lib/Makefile
                       src/lib/ssh/Makefile
                       src/lib/wrt/Makefile
                   examples/Makefile
                   scripts/Makefile
                     scripts/cert/Makefile])

# Outputs the files below
CXXFLAGS="$CXXFLAGS -std=c++0x"
AC_OUTPUT
