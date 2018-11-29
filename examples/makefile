CC = avr-gcc
INCLUDES = -I../../include
LIB = -L../../lib -luart -lspi -lcan -lqueue -lstack -ltimer -lheartbeat
CFLAGS = -std=gnu99 -Wall -g -mmcu=atmega32m1 -Os -mcall-prologues

PGMR = stk500
MCU = m32m1

ifeq ($(OS),Windows_NT)
	# One programmer should give 2 ports (either COM3 and COM4 or COM5 and COM6)
	# Programming port is the higher number (COM4 or COM6)
	# Use powershell, list port names, get the second number in the list
	PORT = $(shell powershell "[System.IO.Ports.SerialPort]::getportnames() | select -First 2 | select -Last 1")
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
