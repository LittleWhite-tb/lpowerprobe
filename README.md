lPowerProbe - A light benchmark tool oriented for energy probing heavely based on likwid

# Compilation

On normal x86 architecture, you can compile the project just using make:
```
    make
```
It is totally possible to redefine the CC/CXX/LD variables to change the compiler used during the process.
You can install the program and the probes using the `install` rule. If you need to choose a specific output folder, you can define the `PREFIX` variable.

## MIC support

lPowerProbe supports MIC. For that, you need to compile using the specialized Makefile:
```
    make -f Makefile.mic
```

## Android support

lPowerProbe supports Android (and ARM). For that, you need to compile using the specialized Makefile:
```
    make -f Makefile.android
```
Your toolchain must be properly configured for crosscompiling to ARM architecture.

# Usage

./lPowerProbe [OPTION] [prog] [args]

Performs standardized and accurate measurements.
It can work in three different modes depending on the prog argument value:
 * prog is not specified: daemon mode: the measurement is (re)started/ended when receiving SIGUSR1.
 * prog ends by .s: it is compiled and run instrumented as a kernel.
 * otherwise: prog is run instrumented as an executable file.

```
  -h, --help				Display this help
  -v, --version				Display the program version

  -l, --libraries			Probes libraries to load to bench the execution
  -r, --repetition=NUMBER		Number of repetition for the test
  -d, --duplicate=NUMBER		Number of process to start
  -p, --pinning="pin1;pin2;..."		Where to pin the processes
  -o, --output="resultFile"		Where to write the results
  -m, --iteration-mem-size=NUMBER	The memory size in bytes used in the kernel per iteration
  -i, --iteration=NUMBER		The number of kernel iteration
```

The result is output in a file called 'output.csv' or in terminal if the file can't be write.


## Generate documentation

The documentation can be generated using doxygen and the following command:
```
   make doc
```
    
## Licence

Copyright (C) 2013-2015 Universite de Versailles
GPLv3

See [COPYRIGHT](https://github.com/LittleWhite-tb/lpowerprobe/blob/master/COPYRIGHT) file


