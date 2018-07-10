# FIR Filters

Finite impulse response (FIR) implemented for ARM processor in C and optimized
in ASM.

## TODO

- [ ] Implementation in C
- [ ] Investigate MLA (multiple and accumulate) instruction
- [ ] Investigate Vector instructions
- [ ] Investigate combining MLA and Vector instructions
- [ ] Optimized ASM
- [ ] Designed hardware instruction

## Building Project

Make is used to build the project. There are two make commands available

Generate ARM assembly

``` shell
make asm
```

Generate ARM binary

``` shell
make binary
```

By default, the `asm` command is run

## Running Project

Use `qemu`
