CC = avr-gcc
CFLAGS = -g -mmcu=atmega32m1 -Os -mcall-prologues

CAN_SRC = $(wildcard *.c)
CAN_OBJ = $(CAN_SRC:%.c=../../build/can/%.o)
CAN_DEP = $(CAN_OBJ:.o=.d)

../../build/can/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

can: $(CAN_OBJ)
	ar rc ../../lib/libcan.a $(CAN_OBJ)

-include $(CAN_DEP)

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean

clean:
	rm -f ../../build/can/*.*

