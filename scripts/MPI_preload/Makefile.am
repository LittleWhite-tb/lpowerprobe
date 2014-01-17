if USE_MPICC
    CXX=mpicc
    CC=mpicc
    LD=mpicc
    lib_LTLIBRARIES = liblppMPInotifier.la
    liblppMPInotifier_la_SOURCES = liblppMPInotifier.c
    liblppMPInotifier_la_LIBADD = ${LIBDL}
    data_subdir = $(datadir)/lPowerProbe
    data_sub_DATA = preloader.sh
endif
