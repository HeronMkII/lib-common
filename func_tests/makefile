CC = avr-gcc
INCLUDES = -I../../include
LIB = -L../../lib -lprintf_flt -lm -ladc -luart -lspi -lcan -lqueue -lstack -ltimer -lheartbeat -lconversions -lpex
CFLAGS = -std=gnu99 -Wall -Wl,-u,vfprintf -g -mmcu=atmega32m1 -Os -mcall-prologues

PGMR = stk500
MCU = m32m1

ifeq ($(OS),Windows_NT)
	PORT = COM3
else
	PORT = $(shell find /dev -name 'tty.usbmodem[0-9]*' | sort | head -n1)
endif

$(PROG): $(PROG).o
	$(CC) $(CFLAGS) -o $@.elf $^ $(LIB)
	avr-objcopy -j .text -j .data -O ihex $@.elf $@.hex

$(PROG).o: $(PROG).c
	$(CC) $(CFLAGS) -c $(PROG).c $(INCLUDES)

.PHONY: clean upload

clean:
	rm -f ./*.o
	rm -f ./*.elf
	rm -f ./*.hex

upload: $(PROG)
	avrdude -p $(MCU) -c $(PGMR) -P $(PORT) -U flash:w:./$^.hex
