if USEMIC
    AM_CPPFLAGS = -DLPP_API_VERSION=2 -mmic
    nobase_lib_LTLIBRARIES = libenergymic/libenergymic.la
    libenergymic_libenergymic_la_SOURCES = mic_energy.c
    libenergymic_libenergymic_la_LIBADD = ${LIBM} ${LIBPTHREAD} ${LIBRT}
endif
