$(LIBNAME)_SRC = $(wildcard *.c)
$(LIBNAME)_OBJ = $($(LIBNAME)_SRC:%.c=../../build/uart/%.o)
$(LIBNAME)_DEP = $($(LIBNAME)_OBJ:.o=.d)

../../build/uart/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBNAME): $($(LIBNAME)_OBJ)
	$(AR) rc ../../lib/lib$(LIBNAME).a $($(LIBNAME)_OBJ)
	$(RANLIB) ../../lib/lib$(LIBNAME).a

-include $($(LIBNAME)_DEP)

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean

clean:
	rm -f ../../build/$(LIBNAME)/*.*

