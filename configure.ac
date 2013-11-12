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
AC_CHECK_LIB([pthread], [pthreade_create], [AC_SUBST(LIBPTHREAD,-lpthread)])
AC_CHECK_LIB([rt], [shm_open], [AC_SUBST(LIBRT,-lrt)])
AC_CHECK_LIB([pfm], [pfm_initialize], [AC_SUBST(LIBPFM,-lpfm)])


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
AC_CHECK_FUNCS([ftruncate gettimeofday memset pow strdup strerror])

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
