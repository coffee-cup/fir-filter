# FIR Filters

Finite impulse response (FIR) implemented for ARM processor in C and optimized
in ASM.

## Building Project

Make is used to build the project. The ARM compiler that is used is based on the
environment variable `ARM_GCC`. When running on the **ugls** lab computers, set
this environment variable with

```shell
export ARM_GCC=/opt/arm/4.3.2/bin/arm-linux-gcc
```

Running

```shell
make
```

will generate the binary `fir` and corresponding assembly output `fir.s`.

## Running Project

```shell
make run
```

will do the above and also run the executable with the `qemu-arm` simulator.
