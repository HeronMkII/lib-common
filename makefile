SUBDIRS = $(addprefix src/,uart spi can timer queue assert)
EXAMPLES = $(dir $(wildcard examples/*/.))

.PHONY: all $(SUBDIRS) clean examples

export CC = avr-gcc
export AR = avr-ar
export RANLIB = avr-ranlib
export INCLUDES = -I../../include
#export LDFLAGS = -L../../lib
export CFLAGS = -Wall -std=gnu99 -g -mmcu=atmega32m1 -Os -mcall-prologues

all: $(SUBDIRS)

clean:
	@$(MAKE) clean -C src/uart
	@$(MAKE) clean -C src/spi
	@$(MAKE) clean -C src/can
	@$(MAKE) clean -C src/timer
	@$(MAKE) clean -C src/queue
	@$(MAKE) clean -C src/assert

$(SUBDIRS):
	@$(MAKE) -e -C $@

examples:
	@for dir in $(EXAMPLES) ; do \
		cd $$dir ; \
		make clean ; \
		make ; \
		cd ../.. ; \
	done
