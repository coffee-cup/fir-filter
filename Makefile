CC = $(ARM_GCC)
CFLAGS = -mfloat-abi=softfp -mfpu=neon -static -O3

TARGET = fir
SRC = main.c
ASM = $(TARGET).s

$(TARGET): $(ASM)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

$(ASM): $(SRC)
	$(CC) $(CFLAGS) -S -o $(ASM) $(SRC)
	# $(CC) $(CFLAGS) -c -g -Wa,-ahl=$(ASM) $(SRC)

run: $(TARGET)
	qemu-arm $(TARGET)

.PHONY: all
all: binary

.PHONY: clean
clean:
	rm -f $(TARGET) $(ASM)
