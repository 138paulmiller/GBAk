# Makefile - to configure set gba to the where gbac is installed 

CC = $(GBAK)/arm/bin/arm-agb-elf-gcc
OBJCOPY = $(GBAK)/arm/bin/arm-agb-elf-objcopy
CRT = $(GBAK)/arm/arm-agb-elf/lib/crt0.o
INC = $(GBAK)/inc
CFLAGS = -std=c99 -O3 -nostartfiles -lm
SRC = $(wildcard $(GBAK)/src/*.c)
ELF = $(SRC:.c=.elf)
OUT = demo

all: $(ELF)
	$(OBJCOPY) -O binary $^ $(OUT).gba
	mv $(ELF) elf/

$(ELF):
	$(CC)  $(CRT) -I$(INC) $(basename $@).c -o $@ $(CFLAGS)


	
clean:
	rm elf/$(ELF) $(OUT).gba
