if USEPFM
    AM_CPPFLAGS = -DLPP_API_VERSION=2
    nobase_lib_LTLIBRARIES = libpfmcounter/libpfmcounter.la
    libpfmcounter_libpfmcounter_la_SOURCES = pfmcounters.c inih/ini.c
    libpfmcounter_libpfmcounter_la_LIBADD = ${LIBPFM}
endif
