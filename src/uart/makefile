CC = avr-gcc
CFLAGS = -g -mmcu=atmega32m1 -Os -mcall-prologues

UART_SRC = $(wildcard *.c)
UART_OBJ = $(UART_SRC:%.c=../../build/uart/%.o)
UART_DEP = $(UART_OBJ:.o=.d)

../../build/uart/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

uart: $(UART_OBJ)
	ar rc ../../lib/libuart.a $(UART_OBJ)

-include $(UART_DEP)

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean

clean:
	rm -f ../../build/uart/*.*

