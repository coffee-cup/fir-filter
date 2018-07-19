CC = $(ARM_GCC)
CFLAGS = -mfloat-abi=softfp -mfpu=neon -static -O3

CCODE = main
CFILE = $(CCODE).c
CASM = $(CCODE).s

binary: $(CFILE)
	$(CC) $(CFLAGS) -o $(CCODE) $(CFILE)

asm: $(CFILE)
	$(CC) $(CFLAGS) -S -o $(CASM) $(CFILE)


run: $(CCODE)
	qemu-arm $(CCODE)

.PHONY: all
all: binary

.PHONY: clean
clean:
	rm -f $(CCODE) $(CASM)
