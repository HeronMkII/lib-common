CC = avr-gcc
CFLAGS = -g -mmcu=atmega32m1 -Os -mcall-prologues

SPI_SRC = $(wildcard *.c)
SPI_OBJ = $(SPI_SRC:%.c=../../build/spi/%.o)
SPI_DEP = $(SPI_OBJ:.o=.d)

../../build/spi/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

spi: $(SPI_OBJ)
	ar rc ../../lib/libspi.a $(SPI_OBJ)

-include $(SPI_DEP)

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean

clean:
	rm -f ../../build/spi/*.*

