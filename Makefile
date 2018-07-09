CC = $(ARM_GCC)
CFLAGS = -mfloat-abi=softfp -mfpu=neon -static -O3

CCODE = main
CFILE = $(CCODE).c
CASM = $(CCODE).s

asm: $(CFILE)
	$(CC) $(CFLAGS) -S -o $(CASM) $(CFILE)

binary: $(CFILE)
	$(CC) $(CFLAGS) --specs=nosys.specs -o $(CCODE) $(CFILE)

.PHONY: all
all: asm

.PHONY: clean
clean:
	rm -f $(CCODE) $(CASM)
