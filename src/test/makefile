LIBNAME = test

SRC = test.c assert.c
OBJ = ../../build/test/test.o ../../build/uart/uart.o ../../build/uart/log.o ../../build/test/assert.o

../../build/test/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

../../build/uart/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: $(OBJ)
	$(AR) rcs ../../lib/libtest.a $(OBJ)
	$(RANLIB) ../../lib/libtest.a

.PHONY: clean

clean:
	-rm -f ../../build/test/*.*
	-rm -f ../../build/uart/*.*

