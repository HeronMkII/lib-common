PROG = uart_print_fp
CC = avr-gcc
INCLUDES = -I../../include
LIB = -L../../lib -luart -lprintf_flt -lm
CFLAGS = -std=gnu99 -Wall -Wl,-u,vfprintf -g -mmcu=atmega32m1 -Os -mcall-prologues

PGMR = stk500
MCU = m32m1
PORT = /dev/tty.usbmodem00187462

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
