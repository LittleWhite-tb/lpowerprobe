CC=gcc
CXX=g++
LD=g++

CFLAGS=-Wextra -Wall -g -O3
LDFLAGS=-ldl -lpthread -lrt

OBJ=src/main.o src/Runner.o src/Experimentation.o  src/Probe.o src/Options.o src/CPUUtils.o
EXEC=lPowerProbe

.PHONY: clean test libs doc

all: $(EXEC) test libs

$(EXEC): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $@

%.o: %.cpp *.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp *.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c *.h
	$(CC) $(CFLAGS) -c $< -o $@

test:
	make -C empty/

libs:
	make -C probes/energy_snb_msr
	make -C probes/wallclock

doc:
	doxygen lPowerProbe.doxy

clean:
	make -C ./empty/ clean
	make -C probes/energy_snb_msr clean
	make -C probes/wallclock clean
	rm -rf $(EXEC)
