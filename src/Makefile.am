AM_CPPFLAGS = -DVERSION=\"$(VERSION)\" -DLIB_DIR=\"$(LIBDIR_DIR)\" -DDATA_DIR=\"$(DATA_DIR)\"
bin_PROGRAMS = $(top_builddir)/lPowerProbe
__top_builddir__lPowerProbe_SOURCES = CPUUtils.cpp \
                                      ExperimentationFactory.cpp ExperimentationResults.cpp ExperimentationResultsWriter.cpp \
                                      Experimentation.cpp DaemonExperimentation.cpp KernelExperimentation.cpp ProgramExperimentation.cpp \
                                      Runner.cpp DaemonRunner.cpp KernelRunner.cpp ProgramRunner.cpp \
                                      KernelCompiler.cpp  Kernel.cpp \
                                      Probe.cpp ProbeDataCollector.cpp ProbeData.cpp ProbeLoader.cpp ProbeV1.cpp ProbeV2.cpp \
                                      main.cpp  Options.cpp RunData.cpp StringUtils.cpp
__top_builddir__lPowerProbe_LDADD = ${LIBM} ${LIBDL} ${LIBRT} ${LIBPTHREAD}

