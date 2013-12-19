#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([lPowerProbe], 2.0, [BUG-REPORT-ADDRESS])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall])
LT_INIT([dlopen])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.
AC_CHECK_LIB([m], [pow], [AC_SUBST(LIBM,-lm)])
AC_CHECK_LIB([dl], [dlopen], [AC_SUBST(LIBDL,-ldl)])
AC_CHECK_LIB([pthread], [pthread_create], [AC_SUBST(LIBPTHREAD,-lpthread)])
AC_CHECK_LIB([rt], [shm_open], [AC_SUBST(LIBRT,-lrt)])


# lmsensors configuration using command line to disable it
AC_ARG_WITH([pfmcounters],
            [AS_HELP_STRING([--without-pfmcounters],
              [disable pfmcounters support])],
            [],
            [with_pfmcounters=yes])

LIBPFM=
          AS_IF([test "x$with_pfmcounters" != xno],
            [AC_CHECK_LIB([pfm], [pfm_initialize],
              [AC_SUBST([LIBPFM], ["-lpfm"])],
              [AC_MSG_WARN(["pfmcounters not found, the probe can't be compiled"])])])

AC_ARG_WITH([yoko],
            [AS_HELP_STRING([--without-yoko],
              [disable YOKO support])],
            [],
            [with_yoko=yes])

YOKOSUPPORT=
          AS_IF([test "x$with_yoko" != xno],
              [AC_SUBST([YOKOSUPPORT], ["yoko"])],
              [])

AC_ARG_WITH([msr],
            [AS_HELP_STRING([--without-msr],
              [disable MSR support])],
            [],
            [with_msr=yes])

MSRSUPPORT=
          AS_IF([test "x$with_msr" != xno],
              [AC_SUBST([MSRSUPPORT], ["msr"])],
              [])

AM_CONDITIONAL(USEPFM,[test $LIBPFM = -lpfm])
AM_CONDITIONAL(USEYOKO,[test $YOKOSUPPORT = yoko])
AM_CONDITIONAL(USEMSR,[test $MSRSUPPORT = msr])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
 
# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([ftruncate gettimeofday memset strdup strerror])

# Check for MPI
AC_CHECK_PROG(MPICC, mpicc, true, false)
AM_CONDITIONAL([USE_MPICC], $MPICC)
if test $MPICC = "false"
then
    AC_MSG_WARN("could not find mpicc")
fi

AC_SUBST(LIBDIR_DIR, $libdir)
AC_SUBST(DATA_DIR, $datadir)

if test -d .git
then
    AC_SUBST(VERSION,[$(git rev-list HEAD --count)"\ \("$(git rev-list HEAD | head -n 1)"\)"])
else
    AC_SUBST(VERSION,"2.0")
fi

# Documentation support

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/Doxyfile doc/Makefile])])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 empty/Makefile
                 scripts/MPI_preload/Makefile
                 probes/Makefile
                 probes/libenergymsr/Makefile
                 probes/libwallclock/Makefile
                 probes/libenergyyoko/Makefile
                 probes/libtimer/Makefile
                 probes/pfmcounters/Makefile])
AC_OUTPUT
