if USEPFM
    AM_CPPFLAGS = -DLPP_API_VERSION=2
    lib_LTLIBRARIES = libpfmcounter.la
    libpfmcounter_la_SOURCES = pfmcounters.c inih/ini.c
    libpfmcounter_la_LIBADD = ${LIBPFM}
endif
