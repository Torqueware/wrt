# Prelude, and project information
AC_INIT([WRT Remote Toolkit], [0.1], [wmillard1@gmail.com], [wrt])
# (Sanity) Check for src folder
AC_CONFIG_SRCDIR([src])
# Use build to hold aux scripts
AC_CONFIG_AUX_DIR([build-aux])
# Start the automake generation process
AM_INIT_AUTOMAKE([1.10 no-define -Wall -Werror foreign])
# Check for g++ (c++) compiler or other programs
AC_PROG_CXX
# Checks for libraries, headers, typedefs, structs, and compiler config
# Checks also for library functions
# Outputs the files below
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
